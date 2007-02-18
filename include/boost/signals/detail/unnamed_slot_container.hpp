// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_UNNAMED_SLOT_CONTAINER_HEADER
#define BOOST_SIGNALS_DETAIL_UNNAMED_SLOT_CONTAINER_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/connect_position.hpp>
#include <list>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // unnamed_slot_container class template.
      // Manages slots for non-grouping signals.
      template<typename T, typename Allocator>
      class unnamed_slot_container
      {
#ifdef BOOST_NO_STD_ALLOCATOR
        typedef std::list<T> list_type;
#else
        typedef std::list<T, typename Allocator::template rebind<T>::other> list_type;
#endif
      public:
        typedef typename list_type::iterator iterator;

        // Constructor (group comparison value is ignored).
        unnamed_slot_container(const no_name_compare&)
        { }

        // Insert a value.
        iterator insert(const T& v, connect_position at)
        {
          if(at == at_back) {
            return list_.insert(v, list_.end());
          } else {
            return list_.insert(v, list_.begin());
          }
        }

        // Erase a value
        iterator erase(iterator pos)
        {
          return list_.erase(pos);
        }

        // Begin and end.
        iterator begin()
        {
          return list_.begin();
        }

        iterator end()
        {
          return list_.end();
        }

        // Retrieve size.
        std::size_t size() const
        {
          return std::count_if(list_.begin(), list_.end(), is_connected());
        }

        // Check for emptiness.
        bool empty() const
        {
          return size() == 0;
        }
      private:
        list_type list_;  
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_UNNAMED_SLOT_CONTAINER_HEADER
