// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SIGNAL_IMPL_HEADER
#define BOOST_SIGNALS_DETAIL_SIGNAL_IMPL_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/unnamed_slot_container.hpp>
#include <boost/signals/detail/named_slot_container.hpp>
#include <boost/signals/detail/slot_signal_interface.hpp>
#include <boost/signals/connection.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // select_slot_container template.
      // Determine the proper type for slot storage from the Group and GroupCompare
      // template arguments.
      template<typename T,
        typename Group,
        typename GroupCompare,
        typename Allocator
      >
      struct select_slot_container
      {
        typedef typename mpl::if_<
          is_same<Group, no_name>,
            unnamed_slot_container<T, Allocator>,
            named_slot_container<T, Group, GroupCompare, Allocator>
        >::type type;
      };

      // signal_impl_base class template.
      template<typename Group,
               typename GroupCompare,
               typename ThreadingModel,
               typename Allocator
      >
      class BOOST_SIGNALS_DECL signal_impl_base 
        : public ThreadingModel::signal_impl_threading_base_type, 
        public enable_shared_from_this<signal_impl_base<Group, 
                                                        GroupCompare, 
                                                        ThreadingModel, 
                                                        Allocator> >,
        public ThreadingModel
      {
      public:
        typedef slot_signal_interface<signal_impl_base> slot_interface;

        signal_impl_base(const GroupCompare& comp)
          : slots_(comp)
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

        // Connect an unnamed slot.
        connection connect_slot(const shared_ptr<slot_interface>& slot,
                                connect_position at)
        {
          local_lock lock(this);
          slot_iterator pos = slots_.insert(slot, at);
          slot->reset(shared_from_this(), pos);
          return connection(slot);
        }

        // Connect a named slot.
        connection connect_slot(const shared_ptr<slot_interface>& slot,
                                const Group& group,
                                connect_position at)
        {
          local_lock lock(this);
          slot_iterator pos = slots_.insert(slot, group, at);
          slot->reset(shared_from_this(), pos);
          return connection(slot);
        }

        // Disconnect a slot group.
        void disconnect(const Group& group)
        {
          local_lock lock(this);
          std::pair<slot_iterator, slot_iterator> range = slots_.group_range(group);
          slot_iterator it = range.first;
          while(it != range.second) {
            slot_iterator tmp = it++;
            (*tmp)->disconnect();
          }
        }
      public:
        typedef typename select_slot_container<shared_ptr<slot_interface>, 
                                               Group, 
                                               GroupCompare,
                                               Allocator>::type slot_container;
        typedef typename slot_container::iterator slot_iterator;

        void remove_slot(const slot_iterator& pos) {
          slots_.erase(pos);
        }

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
