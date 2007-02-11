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
      //   - skips over disconnected slots in the underlying list.
      //   - calls the connected slots when dereferenced.
      //   - caches the result of calling the slots.
      //   - ensures iterator validity.
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

        // The iterator holder for the current position. It ensures that
        // the iterator will be valid, even if dereferencing of this or
        // another slot_call_iterator disconnects the slot.
        struct holder
        {
          // Construct from current and end position.
          holder(iterator_type pos, iterator_type end)
            : pos_(pos), end_(end)
          {
            skip_non_callable();
            if(pos_ != end_) get_slot(pos_)->acquire();
          }
          // Destroy (allow the iterator to become invalid).
          ~holder()
          {
            if(pos_ != end_) get_slot(pos_)->release();
          }
          // Copy.
          holder(const holder& other)
            : pos_(other.pos_), end_(other.end_)
          {
            if(pos_ != end_) get_slot(pos_)->acquire();
          }
          // Assign.
          holder& operator=(const holder& other)
          {
            if(other.pos_ != other.end_) get_slot(other.pos_)->acquire();
            if(pos_ != end_) get_slot(pos_)->release();
            pos_ = other.pos_;
            end_ = other.end_;
            return *this;
          }

          // Increment (advance to next callable slot).
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

          // Skip slots that are not callable (blocked or disconnected).
          void skip_non_callable() {
            while(pos_ != end_ && !get_slot(pos_)->callable())
              ++pos_;
          }

          // Get a pointer to the slot_signal_interface instance.
          // (This is only necessary, because the legacy named_slot_map can't
          //  store those pointers directly, as it is not a template and the
          //  SignalImpl has a named_slot_map as a member.)
          slot_signal_interface<SignalImpl>* get_slot(iterator_type i) {
            return static_cast<slot_signal_interface<SignalImpl>*>((*i).get());
          }

          iterator_type pos_;
          iterator_type end_;
        };

      public:
        // Construct from a slot iterator for the current position, one for the
        // end, a function object to call the slot with the bound arguments and
        // a reference to a cache holding the result.
        slot_call_iterator(iterator_type iter_in, iterator_type end_in, Function f,
                           optional<result_type> &c)
          : holder_(iter_in, end_in), f_(f), cache_(&c)
        { }

        // Dereference the iterator (calls the slot).
        // Since the caller holds the lock of the related signal during
        // the whole iteration, this is the only point where the slots
        // may be modified (recursive locking).
        typename inherited::reference
        dereference() const
        {
          if (!cache_->is_initialized()) {
            cache_->reset(f_(*holder_.pos_));
          }

          return cache_->get();
        }

        // Increment the iterator.
        void increment()
        {
          ++holder_;
          cache_->reset();
        }

        // Compare to another iterator.
        bool equal(const slot_call_iterator& other) const
        {
          // This will be called to decide wether the iterator can be
          // dereferenced, so we need to make sure we're at the end
          // position when there are no more callable slots...
          holder_.skip_non_callable();
          other.holder_.skip_non_callable();
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
