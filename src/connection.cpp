// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/connection.hpp>
#include <cassert>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    // Copy-assignment.
    connection& connection::operator=(const connection& other)
    {
      connection(other).swap(*this);
      return *this;
    }

    // Set the slot's blocking state through this connection.
    void connection::block(bool should_block)
    {
      shared_ptr<slot_connection_interface> sp(slot_.lock());
      if(sp) {
        sp->set_slot_state(0, should_block);
      }
    }

    // Reset the slot's blocking state.
    void connection::unblock()
    {
      shared_ptr<slot_connection_interface> sp(slot_.lock());
      if(sp) {
        bool block = false;
        sp->set_slot_state(0, &block);
      }
    }

    // Retrieve the slot's blocking state.
    bool connection::blocked() const
    {
      shared_ptr<slot_connection_interface> sp(slot_.lock());
      if(sp) {
        bool block;
        sp->get_slot_state(0, &block);
        return block;
      }
      // A disconnected slot is considered not blocked.
      return false;
    }

    // Retrieve the slot connection state.
    bool connection::connected() const
    {
      shared_ptr<slot_connection_interface> sp(slot_.lock());
      if(sp) {
        bool connected;
        sp->get_slot_state(&connected, 0);
        return connected;
      }
      // Slot went out of scope, which makes it disconnected.
      return false;
    }

    // Disconnect the slot from the signal.
    void connection::disconnect() const
    {
      shared_ptr<slot_connection_interface> sp(slot_.lock());
      if(sp) {
        bool connected = false;
        sp->set_slot_state(&connected, 0);
      }
    }

    // Swap two connections.
    void swap(connection& c1, connection& c2)
    {
      c1.swap(c2);
    }

    scoped_connection::scoped_connection(const connection& other) :
      connection(other),
      released_(false)
    {
    }

    scoped_connection::scoped_connection(const scoped_connection& other) :
      connection(other),
      released(other.released_)
    {
    }

    scoped_connection::~scoped_connection()
    {
      if (!released_) {
        this->disconnect();
      }
    }

    connection scoped_connection::release()
    {
      released_ = true;
      return *this;
    }

    void scoped_connection::swap(scoped_connection& other)
    {
      this->connection::swap(other);
      std::swap(this->released_, other.released_);
    }

    void swap(scoped_connection& c1, scoped_connection& c2)
    {
      c1.swap(c2);
    }

    scoped_connection&
    scoped_connection::operator=(const connection& other)
    {
      scoped_connection(other).swap(*this);
      return *this;
    }

    scoped_connection&
    scoped_connection::operator=(const scoped_connection& other)
    {
      scoped_connection(other).swap(*this);
      return *this;
    }
  } // end namespace boost
} // end namespace boost

#ifndef BOOST_MSVC
// Explicit instantiations to keep everything in the library
template class std::list<boost::BOOST_SIGNALS_NAMESPACE::detail::bound_object>;
#endif
