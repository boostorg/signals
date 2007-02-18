// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SLOT_TRACKING_BASE_HEADER
#define BOOST_SIGNALS_DETAIL_SLOT_TRACKING_BASE_HEADER

#include <boost/signals/detail/config.hpp>
#include <boost/signals/detail/slot_connection_interface.hpp>
#include <boost/signals/trackable.hpp>
#include <boost/bind.hpp>
#include <boost/visit_each.hpp>
#include <boost/smart_ptr.hpp>
#include <vector>
#include <algorithm>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      class BOOST_SIGNALS_DECL slot_tracking_base : public slot_connection_interface,
        public enable_shared_from_this<slot_tracking_base>
      {
      public:
        slot_tracking_base(set_function_type s, get_function_type g)
          : slot_connection_interface(s, g)
        { }
        ~slot_tracking_base()
        {
          stop_tracking();
        }

        template<class T>
        void setup_tracking(const T& t) const 
        {
        }
        void start_tracking() const
        {
        }
        void stop_tracking() const
        {
        }
        
        // Check the lifetime of tracked objects.
        bool check_tracked_objects() const {
          return true;
        }
        
      protected:
        typedef std::vector<void*> tracked_pointer_container;

        mutable tracked_pointer_container tracked_pointers_;
      };

      class BOOST_SIGNALS_DECL dual_slot_tracking_base : public slot_tracking_base
      {
      public:
        dual_slot_tracking_base(set_function_type s, get_function_type g)
          : slot_tracking_base(s, g)
        { }
        ~dual_slot_tracking_base()
        {
          stop_tracking();
        }

        // Enumerate and store the trackable-derived objects in a target function.
        template<class T>
        void setup_tracking(const T& t) const 
        {
          trackable_objects_visitor do_bind(trackable_objects_);
          visit_each(do_bind, t);
          this->slot_tracking_base::setup_tracking(t);
        }
        // Establish a connection to the tracked objects.
        void start_tracking() const
        {
          std::for_each(trackable_objects_.begin(), trackable_objects_.end(), 
            bind(&trackable::add_slot, _1, const_cast<dual_slot_tracking_base*>(this)));
          this->slot_tracking_base::start_tracking();
        }
         // Disconnect this slot from all tracked objects.
        void stop_tracking() const
        {
          std::for_each(trackable_objects_.begin(), trackable_objects_.end(),
            bind(&trackable::remove_slot, _1, this));
          trackable_objects_.clear();
          this->slot_tracking_base::stop_tracking();
        }
        
        // Check the lifetime of tracked objects.
        bool check_tracked_objects() const {
          // This is always given with the trackable base approach, so we just
          // forward the call to the base.
          return this->slot_tracking_base::check_tracked_objects();
        }
        
      protected:
        typedef std::vector<const trackable*> trackable_objects_container;

        mutable trackable_objects_container trackable_objects_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_SLOT_TRACKING_BASE_HEADER
