// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SIGNAL_IMPL_HEADER
#define BOOST_SIGNALS_DETAIL_SIGNAL_IMPL_HEADER

#include <boost/signals/detail/config.hpp>
#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/connect_position.hpp>
#include <boost/signals/detail/slot_signal_interface.hpp>
#include <boost/signals/connection.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // signal_impl_base class template.
      template<class ThreadingModel>
      class BOOST_SIGNALS_DECL signal_impl_base : public ThreadingModel 
      {
      public:
        typedef slot_signal_interface<signal_impl_base> slot_interface;

        signal_impl_base()
        { }
        ~signal_impl_base()
        { }

        // Disconnect all slots connected to this signal
        void disconnect_all_slots() {
          local_lock lock(this);
          slot_iterator i = slots_.begin();
          while(i != slots_.end()) {
            slot_iterator tmp = i++;
            if(!(*tmp)->disconnected())
              (*tmp)->release();
          }
        }

        // Are there any connected slots?
        bool empty() const {
          local_lock lock(this);
          return slots_.empty();
        }

        // The number of connected slots
        std::size_t num_slots() const {
          local_lock lock(this);
          return slots_.size();
        }

        connection connect_slot(const shared_ptr<slot_interface>& slot,
                                connect_position at);

      public:
        typedef std::list<shared_ptr<slot_interface> > slot_container;
        typedef typename slot_container::iterator slot_iterator;

        // Slots
        mutable slot_container slots_;
      };

    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_SIGNAL_IMPL_HEADER
