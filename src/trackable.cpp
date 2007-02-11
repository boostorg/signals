// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/trackable.hpp>
#include <algorithm>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    void trackable::remove_slot(const detail::slot_connection_interface* slot) const
    {
      if(!dying_) {
        slot_iterator i = connected_slots_.begin(); 
        while(i != connected_slots_.end())  {
          if(*i != slot) {
              ++i;
          } else {
            i = connected_slots_.erase(i);
          }
        }
      }
    }

    void trackable::add_slot(detail::slot_connection_interface* p) const
    {
      connected_slots_.push_back(p);
    }

    trackable::~trackable()
    {
      bool connected = false;
      dying_ = true;
      for(slot_iterator i = connected_slots_.begin(); 
        i != connected_slots_.end(); ++i)  
      {
          (*i)->set_slot_state(&connected, 0);
      }
    }
  } // end namespace BOOST_SIGNALS_NAMESPACE
}

#ifndef BOOST_MSVC
// Explicit instantiations to keep in the library
template class std::list<weak_ptr<slot_connection_interface> >;
#endif
