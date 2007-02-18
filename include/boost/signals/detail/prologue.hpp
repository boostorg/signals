// Boost.Signals library

// Copyright Timmo Stange 2007. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_PROLOGUE_HEADERS_INCLUDED
#define BOOST_SIGNALS_DETAIL_PROLOGUE_HEADERS_INCLUDED

#  include <boost/config.hpp>
#  include <boost/signals/connection.hpp>
#  include <boost/utility.hpp>
#  include <boost/ref.hpp>
#  include <boost/signals/slot.hpp>
#  include <boost/last_value.hpp>
#  include <boost/signals/detail/type_selection.hpp>
#  include <boost/signals/detail/signal_base.hpp>
#  include <boost/signals/detail/slot_call_iterator.hpp>
#  include <boost/mpl/bool.hpp>
#  include <boost/type_traits/is_convertible.hpp>
#  include <boost/preprocessor/comma_if.hpp>
#  include <boost/preprocessor/enum.hpp>
#  include <boost/preprocessor/enum_params.hpp>
#  include <boost/preprocessor/repetition/enum_binary_params.hpp>
#  include <boost/preprocessor/punctuation/paren.hpp>
#  include <boost/preprocessor/repeat.hpp>
#  include <boost/preprocessor/cat.hpp>
#  include <boost/preprocessor/inc.hpp>
#  include <boost/preprocessor/empty.hpp>
#  include <cassert>
#  include <functional>
#  include <memory>

#endif // ndef BOOST_SIGNALS_DETAIL_PROLOGUE_HEADERS_INCLUDED

// Template parameter list ("typename T0, typename T1, ...")
#define BOOST_SIGNALS_TEMPLATE_PARMS \
  BOOST_PP_ENUM_PARAMS(BOOST_SIGNALS_NUM_ARGS, typename T)

// Template argument list ("T0, T1, T2, ...")
#define BOOST_SIGNALS_TEMPLATE_ARGS \
  BOOST_PP_ENUM_PARAMS(BOOST_SIGNALS_NUM_ARGS, T)

// Template argument variable name pairs list ("T0 a0, T1 a1, ...")
#define BOOST_SIGNALS_PARMS \
  BOOST_PP_ENUM_BINARY_PARAMS(BOOST_SIGNALS_NUM_ARGS, T, a)

// Variable names list ("a0, a1, a2, ...")
#define BOOST_SIGNALS_ARGS \
  BOOST_PP_ENUM_PARAMS(BOOST_SIGNALS_NUM_ARGS, a)

// Bound arguments accessor list ("args->a0, args->a1, ...")
#define BOOST_SIGNALS_BOUND_ARGS \
  BOOST_PP_ENUM_PARAMS(BOOST_SIGNALS_NUM_ARGS, args->a)

#define BOOST_SIGNALS_SEMICOLON ;

// Bound arguments as member declarations ("T0 a0; T1 a1; ...")
#define BOOST_SIGNALS_ARG_AS_MEMBER(z, n, d) \
  BOOST_PP_CAT(T,n) BOOST_PP_CAT(a,n); 
#define BOOST_SIGNALS_ARGS_AS_MEMBERS \
  BOOST_PP_REPEAT(BOOST_SIGNALS_NUM_ARGS,BOOST_SIGNALS_ARG_AS_MEMBER,BOOST_PP_EMPTY)

// Bound arguments as constructor parameter list ("T0 ia0, T1 ia1, ...")
#define BOOST_SIGNALS_COPY_PARMS \
  BOOST_PP_ENUM_BINARY_PARAMS(BOOST_SIGNALS_NUM_ARGS, T, ia)

// Bound arguments as initializer list ("a0(ia0), a1(ia1), a2(ia2)...")
#if BOOST_SIGNALS_NUM_ARGS > 0
#  define BOOST_SIGNALS_INIT_ARG(z, n, d) \
  BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(a,n) ( BOOST_PP_CAT(ia,n) ) 
#  define BOOST_SIGNALS_INIT_ARGS \
  : BOOST_PP_REPEAT(BOOST_SIGNALS_NUM_ARGS, BOOST_SIGNALS_INIT_ARG, BOOST_PP_EMPTY) 
#else
#  define BOOST_SIGNALS_INIT_ARGS
#endif

// Argument type typedefs ("typedef T0 arg1_type; typedef T1 arg2_type; ...")
#define BOOST_SIGNALS_ARG_TYPE(z, n, d) \
  typedef BOOST_PP_CAT(T,n) BOOST_PP_CAT(BOOST_PP_CAT(arg, BOOST_PP_INC(n)),_type); 
#define BOOST_SIGNALS_ARG_TYPES \
  BOOST_PP_REPEAT(BOOST_SIGNALS_NUM_ARGS, BOOST_SIGNALS_ARG_TYPE, BOOST_PP_EMPTY) 

// Function traits member typedefs as template parameters list
// (typename traits::arg1_type, typename traits::arg_type, ...)
#define BOOST_SIGNALS_TRAITS_ARG_TYPE(z, n, d) \
  BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(BOOST_PP_CAT(typename traits::arg, BOOST_PP_INC(n)),_type) 
#define BOOST_SIGNALS_TRAITS_ARG_TYPES \
  BOOST_PP_REPEAT(BOOST_SIGNALS_NUM_ARGS, BOOST_SIGNALS_TRAITS_ARG_TYPE, BOOST_PP_EMPTY) 

// Include the appropriate functionN header
#define BOOST_SIGNAL_FUNCTION_N_HEADER \
  BOOST_JOIN(<boost/function/function,BOOST_SIGNALS_NUM_ARGS.hpp>)
#include BOOST_SIGNAL_FUNCTION_N_HEADER

// Determine if a comma should follow a listing of the arguments/parameters
#if BOOST_SIGNALS_NUM_ARGS == 0
#  define BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
#else
#  define BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS ,
#endif // BOOST_SIGNALS_NUM_ARGS > 0

// Standard library argument type member typedefs
#if BOOST_SIGNALS_NUM_ARGS == 1
#  define BOOST_SIGNALS_STANDARD_ARG_TYPES typedef T0 argument_type;
#elif BOOST_SIGNALS_NUM_ARGS == 2
#  define BOOST_SIGNALS_STANDARD_ARG_TYPES \
  typedef T0 first_argument_type;\
  typedef T1 second_argument_type;
#else
#  define BOOST_SIGNALS_STANDARD_ARG_TYPES
#endif

// Define class names used
#define BOOST_SIGNALS_SIGNAL      BOOST_JOIN(signal,BOOST_SIGNALS_NUM_ARGS)
#define BOOST_SIGNALS_FUNCTION    BOOST_JOIN(function,BOOST_SIGNALS_NUM_ARGS)
#define BOOST_SIGNALS_ARGS_STRUCT BOOST_JOIN(args,BOOST_SIGNALS_NUM_ARGS)
#define BOOST_SIGNALS_CALL_BOUND  BOOST_JOIN(call_bound,BOOST_SIGNALS_NUM_ARGS)
#define BOOST_SIGNALS_SELECT_SLOT_FUNCTION \
   BOOST_JOIN(select_slot_function,BOOST_SIGNALS_NUM_ARGS)

// Define commonly-used instantiations
#define BOOST_SIGNALS_ARGS_STRUCT_INST \
  BOOST_SIGNALS_NAMESPACE::detail::BOOST_SIGNALS_ARGS_STRUCT<BOOST_SIGNALS_TEMPLATE_ARGS>
