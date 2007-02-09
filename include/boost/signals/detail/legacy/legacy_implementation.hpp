// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_LEGACY_LEGACY_IMPLEMENTATION_HEADER
#define BOOST_SIGNALS_DETAIL_LEGACY_LEGACY_IMPLEMENTATION_HEADER

#include <boost/signals/detail/config.hpp>
#include <boost/signals/detail/threading_model.hpp>
#include <boost/signals/detail/legacy/named_slot_map.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // legacy_implementation class.
      // Inherits the aspects of the single_threaded policy and is used
      // for explicit template specialization.
      struct legacy_implementation : single_threaded
      {
        typedef compare_type compare_type;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_LEGACY_LEGACY_IMPLEMENTATION_HEADER
