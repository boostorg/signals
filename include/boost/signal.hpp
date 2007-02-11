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

// Define generation-dependent part of the signature
#  ifndef BOOST_SIGNALS_NO_LEGACY_SUPPORT
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS_WITH_DEFAULT \
      typename Group = int,\
      typename GroupCompare = std::less<Group>
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS typename Group,\
      typename GroupCompare
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS Group, GroupCompare

#  else
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS_WITH_DEFAULT \
    typename ThreadingModel = BOOST_SIGNALS_NAMESPACE::single_threaded
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS typename ThreadingModel
#    define BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS ThreadingModel
#  endif // ndef BOOST_SIGNALS_NO_LEGACY_SUPPORT

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      template<typename Signature,
               typename Combiner,
               BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS,
               typename SlotFunction>
      struct get_signal_impl :
        public real_get_signal_impl<(function_traits<Signature>::arity),
                                    Signature,
                                    Combiner,
                                    BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS,
                                    SlotFunction>
      {
      };

    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE

  // Very lightweight wrapper around the signalN classes that allows signals to
  // be created where the number of arguments does not need to be part of the
  // class name.
  template<
    typename Signature, // function type R (T1, T2, ..., TN)
    typename Combiner = last_value<typename function_traits<Signature>::result_type>,
    BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS_WITH_DEFAULT,
    typename SlotFunction = function<Signature>
  >
  class signal :
    public BOOST_SIGNALS_NAMESPACE::detail::get_signal_impl<Signature, Combiner,
                                                            BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS,
                                                            SlotFunction>::type
  {
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::get_signal_impl<
                       Signature,
                       Combiner,
                       BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS,
                       SlotFunction>::type base_type;

  public:
#  ifdef BOOST_SIGNALS_NO_LEGACY_SUPPORT
    explicit signal(const Combiner& combiner = Combiner())
      : base_type(combiner)
    { }

#  else
    explicit signal(const Combiner& combiner = Combiner(),
                    const GroupCompare& group_compare = GroupCompare()) :
      base_type(combiner, group_compare)
    { }
#  endif // def BOOST_SIGNALS_NO_LEGACY_SUPPORT
  };
} // end namespace boost

#  undef BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS_WITH_DEFAULT
#  undef BOOST_SIGNALS_GENERATION_TEMPLATE_PARMS
#  undef BOOST_SIGNALS_GENERATION_TEMPLATE_ARGS

#endif // ndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNAL_HPP
