// Boost.Signals library

// Copyright Douglas Gregor 2001-2006. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org/libs/signals

#ifndef BOOST_SIGNAL_HPP
#define BOOST_SIGNAL_HPP

#ifndef BOOST_SIGNALS_MAX_ARGS
#  define BOOST_SIGNALS_MAX_ARGS 10
#endif

#include <boost/config.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/signals/signal0.hpp>
#include <boost/signals/signal1.hpp>
#include <boost/signals/signal2.hpp>
#include <boost/signals/signal3.hpp>
#include <boost/signals/signal4.hpp>
#include <boost/signals/signal5.hpp>
#include <boost/signals/signal6.hpp>
#include <boost/signals/signal7.hpp>
#include <boost/signals/signal8.hpp>
#include <boost/signals/signal9.hpp>
#include <boost/signals/signal10.hpp>
#include <boost/function.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

#ifndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      template<typename Signature,
               typename Combiner,
               typename Group,
               typename GroupCompare,
               typename SlotFunction,
               typename ThreadingModel,
               typename Allocator
      >
      struct get_signal_impl :
        public real_get_signal_impl<(function_traits<Signature>::arity),
                                    Signature,
                                    Combiner,
                                    Group,
                                    GroupCompare,
                                    SlotFunction,
                                    ThreadingModel,
                                    Allocator>
      {
      };

    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE

  // Very lightweight wrapper around the signalN classes that allows signals to
  // be created where the number of arguments does not need to be part of the
  // class name.
  template<
    typename Signature, // function type R (T1, T2, ..., TN)
    typename Combiner       = BOOST_SIGNALS_NAMESPACE::use_default,
    typename Group          = BOOST_SIGNALS_NAMESPACE::use_default,
    typename GroupCompare   = BOOST_SIGNALS_NAMESPACE::use_default,
    typename SlotFunction   = BOOST_SIGNALS_NAMESPACE::use_default,
    typename ThreadingModel = BOOST_SIGNALS_NAMESPACE::use_default,
    typename Allocator      = BOOST_SIGNALS_NAMESPACE::use_default
  >
  class signal :
    public BOOST_SIGNALS_NAMESPACE::detail::get_signal_impl<Signature, 
                                                            Combiner,
                                                            Group,
                                                            GroupCompare,
                                                            SlotFunction,
                                                            ThreadingModel,
                                                            Allocator>::type
  {
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::get_signal_impl<
                       Signature,
                       Combiner,
                       Group,
                       GroupCompare,
                       SlotFunction,
                       ThreadingModel,
                       Allocator>::type base_type;

  public:
    explicit signal(const typename base_type::combiner_type& 
      combiner = combiner_type(),
      const typename base_type::group_compare_type& 
      group_compare = group_compare_type()) 
      : base_type(combiner, group_compare)
    { }
  };
} // end namespace boost

#endif // ndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNAL_HPP
