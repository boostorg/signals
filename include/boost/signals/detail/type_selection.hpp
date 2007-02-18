// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_TYPE_SELECTION_HEADER
#define BOOST_SIGNALS_DETAIL_TYPE_SELECTION_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/signal_base.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

#ifndef BOOST_SIGNALS_DEFAULT_ALLOCATOR
// NOTE: We should check if a standard allocator is available here.
#  define BOOST_SIGNALS_DEFAULT_ALLOCATOR std::allocator<int>
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    // use_default type.
    // Used to leave signal template arguments to their default value while
    // specializing arguments that are further behind in the list of template
    // parameters.
    struct use_default;

    namespace detail {

      // select_group template structure.
      // Determine the appropriate group type from the Group template argument.
      template<typename Group>
      struct select_group
      {
        typedef Group type;
      };

      template<>
      struct select_group<use_default>
      {
        typedef int type;
      };

      // select_group_compare template structure.
      // Determine the appropriate group compare type from the Group and GroupCompare 
      // template arguments.  
      template<typename Group, typename GroupCompare>
      struct select_group_compare
      {
        typedef typename mpl::if_<
          is_same<Group, no_name>,
            no_name_compare,
            typename mpl::if_<
              is_same<GroupCompare, use_default>,
                std::less<typename select_group<Group>::type>,
                GroupCompare
            >::type
          >::type type;
      };

      // select_combiner template structure.
      // Determine the appropriate combiner type from the Combiner and R template
      // arguments.  
      template<typename Combiner, typename R>
      struct select_combiner
      {
        typedef typename mpl::if_<
          is_same<Combiner, use_default>,
            last_value<R>,
            Combiner
          >::type type;
      };

      // select_threading_model template structure.
      // Determine the appropriate threading model type from the ThreadingModel
      // template argument.
      template<typename ThreadingModel>
      struct select_threading_model {
        typedef ThreadingModel type;
      };

      template<>
      struct select_threading_model<use_default> {
        typedef single_threaded type;
      };

      // select_allocator template structure.
      // Determine the appropriate allocator type from the Allocator
      // template argument.
      template<typename Allocator>
      struct select_allocator {
        typedef Allocator type;
      };

      template<>
      struct select_allocator<use_default> {
        typedef BOOST_SIGNALS_DEFAULT_ALLOCATOR type;
      };

      // select_signal_base template structure.
      // Determine the appropriate signal base class from the signal
      // template arguments.
      template<typename Combiner, 
        typename R,
        typename Group, 
        typename GroupCompare, 
        typename ThreadingModel, 
        typename Allocator>
      struct select_signal_base {
        typedef signal_base<
          typename select_combiner<Combiner, R>::type,
          typename select_group<Group>::type,
          typename select_group_compare<Group, GroupCompare>::type,
          typename select_threading_model<ThreadingModel>::type,
          typename select_allocator<Allocator>::type
        > type;
      };

      // select_result_type template structure.
      // Determine the result type of a slot call.
      template<typename R>
      struct select_result_type {
        typedef R type;
      };

      template<>
      struct select_result_type<void> {
        typedef unusable type;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_TYPE_SELECTION_HEADER
