// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SLOT_CONNECTION_INTERFACE_HPP
#define BOOST_SIGNALS_DETAIL_SLOT_CONNECTION_INTERFACE_HPP

#include <boost/noncopyable.hpp>
#include <boost/signals/detail/signals_common.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // slot_connection_interface class.
      // Provides the interface for connections and bound objects to observe and
      // manipulate the slot they're associate with. Base class for all slot types.
      class slot_connection_interface : public noncopyable
      {
      public:
        // Manipulating the slot state.
        void set_slot_state(bool* connected, bool* blocked) {
          (*set_state_function_)(this, connected, blocked);
        }
        // Observing the slot state.
        void get_slot_state(bool* connected, bool* blocked) const {
          (*get_state_function_)(this, connected, blocked);
        }

      protected:
        // Interface function types for the derived slot classes.
        typedef void (*set_function_type)(slot_connection_interface*, 
                                          bool*, bool*);
        typedef void (*get_function_type)(const slot_connection_interface*, 
                                          bool*, bool*);

        // Construct from a pair of interface functions.
        slot_connection_interface(set_function_type set, get_function_type get)
          : set_state_function_(set), get_state_function_(get)
        { }

      private:
        set_function_type set_state_function_;
        get_function_type get_state_function_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SLOT_CONNECTION_INTERFACE_HPP
