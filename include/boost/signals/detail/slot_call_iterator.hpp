// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_SLOT_CALL_ITERATOR
#define BOOST_SIGNALS_SLOT_CALL_ITERATOR

#include <memory>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/signals/detail/config.hpp>
#include <boost/signals/connection.hpp>
#include <boost/optional.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {

      // Generates a slot call iterator. Essentially, this is an iterator that:
      //   - skips over disconnected slots in the underlying list
      //   - calls the connected slots when dereferenced
      //   - caches the result of calling the slots
      template<typename Function, typename SignalImpl>
      class slot_call_iterator
        : public iterator_facade<slot_call_iterator<Function, SignalImpl>,
                                 typename Function::result_type,
                                 single_pass_traversal_tag,
                                 typename Function::result_type const&>
      {
        typedef iterator_facade<slot_call_iterator<Function, SignalImpl>,
                                typename Function::result_type,
                                single_pass_traversal_tag,
                                typename Function::result_type const&>
          inherited;

        typedef typename SignalImpl::slot_iterator iterator_type;
        typedef typename Function::result_type result_type;

        friend class iterator_core_access;

        struct holder
        {
          holder(iterator_type pos, iterator_type end)
            : pos_(pos), end_(end)
          {
            skip_non_callable();
            if(pos_ != end_) get_slot(pos_)->acquire();
          }
          ~holder()
          {
            if(pos_ != end_) get_slot(pos_)->release();
          }
          holder(const holder& other)
            : pos_(other.pos_), end_(other.end_)
          {
            if(pos_ != end_) get_slot(pos_)->acquire();
          }
          holder& operator=(const holder& other)
          {
            if(other.pos_ != other.end_) get_slot(other.pos_)->acquire();
            if(pos_ != end_) get_slot(pos_)->release();
            pos_ = other.pos_;
            end_ = other.end_;
            return *this;
          }

          holder& operator++()
          {
            if(pos_ != end_) {
              iterator_type tmp = pos_++;
              skip_non_callable();
              if(pos_ != end_)
                get_slot(pos_)->acquire();
              get_slot(tmp)->release();
            }
            return *this;
          }

          void skip_non_callable() {
            while(pos_ != end_ && 
              (get_slot(pos_)->disconnected() || get_slot(pos_)->blocked()))
              ++pos_;
          }

          slot_signal_interface<SignalImpl>* get_slot(iterator_type i) {
            return static_cast<slot_signal_interface<SignalImpl>*>((*i).get());
          }

          iterator_type pos_;
          iterator_type end_;
        };

      public:
        slot_call_iterator(iterator_type iter_in, iterator_type end_in, Function f,
                           optional<result_type> &c)
          : holder_(iter_in, end_in), f_(f), cache_(&c)
        { }

        typename inherited::reference
        dereference() const
        {
          if (!cache_->is_initialized()) {
            cache_->reset(f_(*holder_.pos_));
          }

          return cache_->get();
        }

        void increment()
        {
          ++holder_;
          cache_->reset();
        }

        bool equal(const slot_call_iterator& other) const
        {
          return holder_.pos_ == other.holder_.pos_;
        }

      private:
        mutable holder holder_;
        Function f_;
        optional<result_type>* cache_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SLOT_CALL_ITERATOR
