// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_CONNECTION_HPP
#define BOOST_SIGNALS_CONNECTION_HPP

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/slot_connection_interface.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/operators.hpp>
#include <boost/any.hpp>
#include <list>
#include <cassert>
#include <utility>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    class trackable;

    namespace detail {
      // Represents an object that has been bound as part of a slot, and how
      // to notify that object of a disconnect
      struct bound_object {
        void* obj;
        void* data;
        void (*disconnect)(void*, void*);

        bool operator==(const bound_object& other) const
          { return obj == other.obj && data == other.data; }
        bool operator<(const bound_object& other) const
          { return obj < other.obj; }

        // To support intel 80 compiler, 2004/03/18 (Mark Rodgers)
        bool operator!=(const bound_object& other) const
        { return !(*this==other); }
        bool operator>(const bound_object& other) const
        { return !(*this < other); }
      };

      // Describes the connection between a signal and the objects that are
      // bound for a specific slot. Enables notification of the signal and the
      // slots when a disconnect is requested.
      struct basic_connection {
        void* signal;
        void* signal_data;
        void (*signal_disconnect)(void*, void*);
        bool blocked_;

        std::list<bound_object> bound_objects;
      };

    } // end namespace detail

    // connection class.
    // The user may freely pass around the "connection" object and terminate
    // the connection at any time using disconnect().
    class BOOST_SIGNALS_DECL connection :
      private less_than_comparable1<connection>,
      private equality_comparable1<connection>
    {
    public:
      // Default construction creates a connection that's not associated with
      // a slot.
      connection() : slot_()
      { }
      // Construction from a shared_ptr to a slot.
      explicit connection(const shared_ptr<
        BOOST_SIGNALS_NAMESPACE::detail::slot_connection_interface
      >& slot) 
        : slot_(slot)
      { }
      // Copy construction.
      connection(const connection& other) 
        : slot_(other.slot_)
      { }
      // Destruction.
      ~connection()
      { }

      // Block he connection: if the connection is still active, there
      // will be no notification.
      void block(bool should_block = true);
      // Lift the block.
      void unblock();
      // Check blocking state.
      bool blocked() const;

      // Disconnect the signal and slot, if they are connected.
      void disconnect() const;

      // Returns true if the signal and slot are connected.
      bool connected() const;

      // Comparison of connections.
      bool operator==(const connection& other) const {
        return !(slot_ < other.slot_) && !(other.slot_ < slot_);
      }
      bool operator<(const connection& other) const {
        return slot_ < other.slot_;
       }

      // Connection assignment.
      connection& operator=(const connection& other) {
        slot_ = other.slot_;
        return *this;
      }

      // Swap connections.
      void swap(connection& other) {
        this->slot_.swap(other.slot_);
       }

    private:
      // Pointer to the slot-connection interface.
      weak_ptr<BOOST_SIGNALS_NAMESPACE::detail::slot_connection_interface> 
        slot_;
    };

    // scoped_connection class.
    // Similar to connection, but will disconnect the connection when it is
    // destroyed unless release() has been called.
    class BOOST_SIGNALS_DECL scoped_connection : public connection {
    public:
      // Default construction.
      scoped_connection() 
        : connection(), released_(false) 
      { }
      // Construction from base.
      scoped_connection(const connection&);
      // Copy construction.
      scoped_connection(const scoped_connection&);
      // Destruction.
      ~scoped_connection();

      // Relieve the connection from control over the slot.
      connection release();

      inline void swap(scoped_connection&);

      // Assignment operations.
      scoped_connection& operator=(const connection&);
      scoped_connection& operator=(const scoped_connection&);

    private:
      bool released_;
    };
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_CONNECTION_HPP
