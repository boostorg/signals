// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/detail/legacy/named_slot_map.hpp>
#include <cassert>
#include <map>
#include <list>
#include <typeinfo>

namespace boost { namespace BOOST_SIGNALS_NAMESPACE { namespace detail {

  typedef std::list<shared_ptr<slot_connection_interface> > group_list;
  typedef group_list::iterator slot_pair_iterator;
  typedef std::map<stored_group, group_list, compare_type> slot_container_type;
  typedef slot_container_type::iterator group_iterator;
  typedef slot_container_type::const_iterator const_group_iterator;


#if BOOST_WORKAROUND(_MSC_VER, <= 1400)
  void named_slot_map_iterator::decrement() { assert(false); }
  void named_slot_map_iterator::advance(difference_type) { assert(false); }
#endif

  named_slot_map::named_slot_map(const compare_type& compare) : groups(compare)
  {
    clear();
  }

  void named_slot_map::clear()
  {
    groups.clear();
    groups[stored_group(stored_group::sk_front)];
    groups[stored_group(stored_group::sk_back)];
    back = groups.end();
    --back;
  }

  named_slot_map::iterator named_slot_map::begin()
  {
    return named_slot_map::iterator(groups.begin(), groups.end());
  }

  named_slot_map::iterator named_slot_map::end()
  {
    return named_slot_map::iterator(groups.end(), groups.end());
  }

  named_slot_map::iterator
    named_slot_map::insert(const stored_group& name, 
      const shared_ptr<slot_connection_interface>& slot,
      connect_position at)
  {
    group_iterator group;
    if (name.empty()) {
      switch (at) {
    case at_front: group = groups.begin(); break;
    case at_back: group = back; break;
      }
    } else {
      group = groups.find(name);
      if (group == groups.end()) {
        slot_container_type::value_type v(name, group_list());
        group = groups.insert(v).first;
      }
    }
    iterator it;
    it.group = group;
    it.last_group = groups.end();

    switch (at) {
    case at_back:
      group->second.push_back(slot);
      it.slot_ = group->second.end();
      it.slot_assigned = true;
      --(it.slot_);
      break;

    case at_front:
      group->second.push_front(slot);
      it.slot_ = group->second.begin();
      it.slot_assigned = true;
      break;
    }
    return it;
  }

  void named_slot_map::disconnect(const stored_group& name)
  {
    group_iterator group = groups.find(name);
    if (group != groups.end()) {
      slot_pair_iterator i = group->second.begin();
      bool connected = false;
      while (i != group->second.end()) {
        slot_pair_iterator next = i;
        ++next;
        (*i)->set_slot_state(&connected, 0);
        i = next;
      }
      groups.erase(group);
    }
  }

  void named_slot_map::erase(iterator pos)
  {
    // Erase the slot
    pos.group->second.erase(pos.slot_);
  }

  struct is_connected {
    bool operator()(const shared_ptr<slot_connection_interface>& slot) const {
      bool c;
      slot->get_slot_state(&c, 0);
      return c;
    }
  };

  // TODO: Get rid of the const_cast<>s
  std::size_t named_slot_map::count() const
  {
    return std::count_if(
      const_cast<named_slot_map*>(this)->begin(), 
      const_cast<named_slot_map*>(this)->end(), is_connected());
  }
} } }
