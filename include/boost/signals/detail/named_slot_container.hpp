// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_NAMED_SLOT_CONTAINER_HEADER
#define BOOST_SIGNALS_DETAIL_NAMED_SLOT_CONTAINER_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/connect_position.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <list>
#include <map>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // named_slot_container class template.
      // Manages slots for grouping signals.
      template<typename T,
        typename Group,
        typename GroupCompare,
        typename Allocator>
      class named_slot_container
      {
        // The internal map needs to differentiate between named and
        // unnamed slots. Unnamed slots are either positioned at the
        // front or back of the map.
        struct key
        {
          enum group_type { type_front, type_group, type_back };

          // Construct a key from a group name. This is for a named slot.
          key(const Group& group)
            : type_(type_group), group_(group)
          { }

          // Construct a key from a position. This is for an unnamed slot.
          key(connect_position at)
            : group_()
          {
            if(at == at_back)
              type_ = type_back;
            else
              type_ = type_front;
          }

          group_type type_;
          Group group_;
        };

        // The comparison operation for internal keys
        struct key_compare
        {
          // Construct from a group comparison operator.
          key_compare(const GroupCompare& group_compare)
            : group_compare_(group_compare)
          { }

          bool operator()(const key& lhs, const key& rhs) const
          {
            // Both keys are for named slots. Compare the names.
            if(lhs.type_ == key::type_group && rhs.type_ == key::type_group)
              return group_compare_(lhs.group_, rhs.group_);
            // If lhs is a key for an unnamed back slot, it belongs to the end
            // of the map. If the other key is for an unnamed front slot, it
            // belongs behind it.
            if(lhs.type_ == key::type_back || rhs.type_ == key::type_front)
              return false;
            // If the rhs key is for an unnamed back slot, the lhs key belongs
            // in front of it.
            if(rhs.type_ == key::type_back)
              return true;
          }

          GroupCompare group_compare_;
        };

        // Internal storage is a map of lists of slots.
#ifdef BOOST_NO_STD_ALLOCATOR
        typedef std::list<T> list_type;
        typedef std::map<key, list_type, key_compare> map_type;
#else
        typedef std::list<T, typename Allocator::template rebind<T>::other> list_type;
        typedef std::map<key, list_type, key_compare, 
          typename Allocator::template rebind<list_type>::other> map_type;
#endif

      public:
        // The named_slot_container iterator class.
        class iterator : public iterator_facade<iterator,
                                                T,
                                                forward_traversal_tag>
        {
        public:
          // Copy construction.
          iterator(const iterator& other)
            : map_pos_(other.map_pos_), list_pos_(other.list_pos_), 
            map_end_(other.map_end_)
          { }

          // Copy assignment.
          iterator& operator=(const iterator& other)
          {
            map_pos_ = other.map_pos_;
            list_pos_ = other.list_pos_;
            map_end_ = other.map_end_;
            return *this;
          }

        private:
          friend class iterator_core_access;
          friend named_slot_container;

          // Construct from a map iterator a list iterator and the map's end.
          iterator(const typename map_type::iterator map_it,
            const typename list_type::iterator list_it,
            const typename map_type::iterator map_end)
            : map_pos_(map_it), list_pos_(list_it), map_end_(map_end)
          { }

          T& dereference() const
          {
            return *list_pos_;
          }

          void increment() 
          {
            if(++list_pos_ == map_pos_->second.end()) {
              ++map_pos_;
              if(map_pos_ != map_end_)
                list_pos_ = map_pos_->second.begin();
            }
          }

          bool equal(const iterator& other) const
          {
            if(map_pos_ != other.map_pos_)
              return false;
            if(map_pos_ == map_end_)
              return true;
            return list_pos_ == other.list_pos_;
          }

          typename map_type::iterator map_pos_;
          typename map_type::const_iterator map_end_;
          typename list_type::iterator list_pos_;
        };

        // Constructor (group comparison value is ignored).
        named_slot_container(const GroupCompare& comp)
          : map_(key_compare(comp))
        { }

        // Insert a value.
        iterator insert(const T& v, connect_position at)
        {
          return do_insert(v, key(at), at);
        }

        // Insert a value with a given group name.
        iterator insert(const T& v, const Group& group, connect_position at)
        {
          return do_insert(v, key(group), at);
        }

        // Erase a single entry.
        iterator erase(iterator pos)
        {
          iterator tmp = pos++;
          tmp.map_pos_->second.erase(tmp.list_pos_);
          if(tmp.map_pos_->second.empty())
            map_.erase(tmp.map_pos_->first);
          return pos;
        }

        // Begin and end.
        iterator begin()
        {
          if(!map_.empty())
            return iterator(map_.begin(), map_.begin()->second.begin(), map_.end());
          return iterator(map_.begin(), typename list_type::iterator(), map_.end());
        }

        iterator end()
        {
          return iterator(map_.end(), typename list_type::iterator(), map_.end());
        }

        // Retrieve size.
        std::size_t size() const
        {
          return count_if(begin(), end(), is_connected());
        }

        // Check for emptiness.
        bool empty() const
        {
          return size() == 0;
        }

        // Get an iterator pair for a specific group range.
        std::pair<iterator, iterator> group_range(const Group& group) {
          typename map_type::iterator map_it = map_.find(key(group));
          if(map_it == map_.end()) {
            return std::make_pair(end(), end());
          }
          typename map_type::iterator next = map_it;
          ++next;
          if(next == map_.end()) {
            return std::make_pair(
              iterator(map_it, map_it->second.begin(), map_.end()),
              end());
          }
          return std::make_pair(
            iterator(map_it, map_it->second.begin(), map_.end()),
            iterator(next, next->second.begin(), map_.end()));
        }

      private:
        iterator do_insert(const T& v, const key& k, connect_position at)
        {
          typename map_type::iterator map_it = map_.find(k);
          if(map_it == map_.end()) {
            // Insert an empty list for the given key into the map.
            map_it = map_.insert(k, list_type());
          }

          // Insert the value into the appropriate list.
          typename list_type::iterator list_it;
          if(at == at_back) {
            list_it = map_it->second.insert(v, map_it->second.end());
          } else {
            list_it = map_it->second.insert(v, map_it->second.begin());
          }

          return iterator(map_it, list_it, map_.end());
        }

        map_type map_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_NAMED_SLOT_CONTAINER_HEADER
