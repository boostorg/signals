// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_LEGACY_SLOT_TRACKING_BASE_HEADER
#define BOOST_SIGNALS_DETAIL_LEGACY_SLOT_TRACKING_BASE_HEADER

#include <boost/signals/detail/slot_connection_interface.hpp>
#include <boost/signals/trackable.hpp>
#include <boost/bind.hpp>
#include <boost/visit_each.hpp>
#include <vector>
#include <algorithm>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      class legacy_slot_tracking_base : public slot_connection_interface
      {
      public:
        legacy_slot_tracking_base(set_function_type s, get_function_type g)
          : slot_connection_interface(s, g)
        { }
        ~legacy_slot_tracking_base()
        {
          stop_tracking();
        }

        // Enumerate and store the trackable-derived objects in a target function.
        template<class T>
        void start_tracking(const T& t) 
        {
          bound_objects_visitor do_bind(bound_objects_);
          visit_each(do_bind, t);
          std::for_each(bound_objects_.begin(), bound_objects_.end(), 
            bind(&trackable::add_slot, _1, shared_from_this()));
        }
        // Disconnect this slot from all tracked objects.
        void stop_tracking() 
        {
          std::for_each(bound_objects_.begin(), bound_objects_.end(),
            bind(&trackable::remove_slot, _1, this));
          bound_objects_.clear();
        }
      private:
        std::vector<const trackable*> bound_objects_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_LEGACY_SLOT_TRACKING_BASE_HEADER
