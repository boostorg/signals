// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_LEGACY_SIGNAL_IMPL_BASE_HEADER
#define BOOST_SIGNALS_DETAIL_LEGACY_SIGNAL_IMPL_BASE_HEADER

#include <boost/signals/detail/signal_impl_base.hpp>
#include <boost/signals/detail/legacy/legacy_implementation.hpp>
#include <boost/signals/detail/legacy/named_slot_map.hpp>
#include <boost/signals/detail/legacy/slot_tracking_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // signal_impl_base explicit specialization.
      template<>
      class signal_impl_base<legacy_implementation> : public legacy_implementation
      {
      public:
        typedef named_slot_map::iterator slot_iterator;
        typedef legacy_slot_tracking_base slot_tracker_base_type;

        bool empty() const {
          return slots_.count() == 0;
        }

        std::size_t num_slots() const {
          return slots_.count();
        }

        void disconnect_all_slots() {
          slot_iterator i = slots_.begin();
          bool connected = false;
          while(i != slots_.end()) {
            slot_iterator tmp = i++;
            (*tmp)->set_slot_state(&connected, 0);
          }
        }

       typedef compare_type compare_type;

     protected:
        signal_impl_base(const compare_type& comp)
          : slots_(comp)
        { }
        ~signal_impl_base()
        { }

      private:
        mutable named_slot_map slots_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_LEGACY_SIGNAL_IMPL_BASE_HEADER
