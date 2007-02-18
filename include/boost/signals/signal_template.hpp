// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

// This file intentionally does not have include guards, because it is meant
// to be included multiple times (one for each signalN class). The
// BOOST_SIGNALS_SIGNAL_TEMPLATE_HEADER_INCLUDED macro merely serves to
// suppress reinclusion of the files that this header depends on.

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

// Includes and macro setup.
#include <boost/signals/detail/prologue.hpp>

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // select_slot_function meta function.
      template<typename SlotFunction, typename R
        BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
        BOOST_SIGNALS_TEMPLATE_PARMS>
      struct BOOST_SIGNALS_SELECT_SLOT_FUNCTION {
        typedef typename mpl::if_<
          is_same<SlotFunction, use_default>,
            BOOST_SIGNALS_FUNCTION<R 
                                   BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS 
                                   BOOST_SIGNALS_TEMPLATE_ARGS>,
            SlotFunction
        >::type type;
      };

      // Holds the arguments for a bound slot call in a single place
      template<BOOST_SIGNALS_TEMPLATE_PARMS
               BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
               typename Dummy = int>
      struct BOOST_SIGNALS_ARGS_STRUCT {
        BOOST_SIGNALS_ARGS_STRUCT(BOOST_SIGNALS_COPY_PARMS)
          BOOST_SIGNALS_INIT_ARGS
        {
        }

        BOOST_SIGNALS_ARGS_AS_MEMBERS
      };

      // Function object that calls the function object given to it, passing
      // the bound arguments along to that underlying function object
      template<typename R>
      struct BOOST_SIGNALS_CALL_BOUND {
        template<
          BOOST_SIGNALS_TEMPLATE_PARMS
          BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
          typename F,
          typename SlotConnection
        >
        struct caller {
          typedef BOOST_SIGNALS_ARGS_STRUCT<BOOST_SIGNALS_TEMPLATE_ARGS>*
            args_type;

          args_type args;

          typedef R result_type;

          caller() {}
          caller(args_type a) : args(a) {}

          template<typename SlotPtr>
          R operator()(const SlotPtr& ptr) const
          {
            const SlotConnection* slot =
              static_cast<const SlotConnection*>(ptr.get());
            F* target = const_cast<F*>(&(slot->get_slot_function()));
            return (*target)(BOOST_SIGNALS_BOUND_ARGS);
          }
        };
      };

      template<>
      struct BOOST_SIGNALS_CALL_BOUND<void> {
        template<
          BOOST_SIGNALS_TEMPLATE_PARMS
          BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
          typename F,
          typename SlotConnection
        >
        struct caller {
          typedef BOOST_SIGNALS_ARGS_STRUCT<BOOST_SIGNALS_TEMPLATE_ARGS>*
            args_type;

          args_type args;

          typedef unusable result_type;

          caller(args_type a) : args(a) {}

          template<typename SlotPtr>
          unusable operator()(const SlotPtr& ptr) const
          {
            const SlotConnection* slot =
              static_cast<const SlotConnection*>(ptr.get());
            F* target = const_cast<F*>(&(slot->get_slot_function()));
            (*target)(BOOST_SIGNALS_BOUND_ARGS);
            return unusable();
          }
        };
      };
    } // namespace detail
  } // namespace BOOST_SIGNALS_NAMESPACE

  // signalN class template.
  // Template for the actual signal, where N is the number of parameters that are
  // used for signal and slot invocation.
  //
  // Template parameters:
  //  R                 - Return type of a slot connected to the signal.
  //  T0, T1,... , TN-1 - Types of the N call parameters.
  //  Combiner          - Result combiner algorithm. Type of callable entity that takes 
  //                      a range of slot invocations in the form of two input iterators
  //                      as arguments. Defaults to boost::last_value<R>.
  //  Group             - The type by which slots for this signal can be grouped.
  //                      Defaults to int. Specifying signals::no_name results in
  //                      better performance, but does not allow grouping of slots.
  //  GroupCompare      - Ordering predicate for the slot groups. Defaults to
  //                      std::less<Group>.
  //  SlotFunction      - A generalized function object wrapper used to store the slot
  //                      targets. Defaults to boost::funcionN with the appropriate
  //                      signature.
  //  ThreadingModel    - Thread-safety parameterization. Defaults to signals::single_
  //                      threaded, which provides no thread-safety. Specifying
  //                      signals::multi_threaded synchronizes signal invocation and
  //                      manipulation between threads.
  //  Allocator         - Allocator type used for internal storage of slots. Defaults
  //                      to BOOST_SIGNALS_DEFAULT_ALLOCATOR.
  template<
    typename R,
    BOOST_SIGNALS_TEMPLATE_PARMS
    BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
    typename Combiner       = BOOST_SIGNALS_NAMESPACE::use_default,
    typename Group          = BOOST_SIGNALS_NAMESPACE::use_default,
    typename GroupCompare   = BOOST_SIGNALS_NAMESPACE::use_default,
    typename SlotFunction   = BOOST_SIGNALS_NAMESPACE::use_default,
    typename ThreadingModel = BOOST_SIGNALS_NAMESPACE::use_default,
    typename Allocator      = BOOST_SIGNALS_NAMESPACE::use_default
  >
  class BOOST_SIGNALS_SIGNAL :
    public BOOST_SIGNALS_NAMESPACE::detail::select_signal_base<
      Combiner, R, Group, GroupCompare, ThreadingModel, Allocator>::type
  {
  private:
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_signal_base<
      Combiner, R, Group, GroupCompare, ThreadingModel, Allocator>::type 
      base_type;              // The signal base type.

    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_threading_model<
      ThreadingModel>::type
      threading_model_type;

  public:
    // Public interface member typedefs.
    // Combiner type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_combiner<
      Combiner, R>::type 
      combiner_type;

    // Signal result type.
    typedef typename combiner_type::result_type 
      result_type;

    // Group type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_group<
      Group>::type 
      group_type;

    // Group compare type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_group_compare<
      Group, GroupCompare>::type 
      group_compare_type;

    // Slot function type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::BOOST_SIGNALS_SELECT_SLOT_FUNCTION<
      SlotFunction, 
      R 
      BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS 
      BOOST_SIGNALS_TEMPLATE_ARGS>::type 
      slot_function_type;

    // Slot model type.
    typedef slot<slot_function_type> 
      slot_type;

    // Slot result type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_result_type<
      R>::type 
      slot_result_type;

    // Allocator type.
    typedef typename BOOST_SIGNALS_NAMESPACE::detail::select_allocator<
      Allocator>::type 
      allocator_type;

  private:
    // Slot connection type
    typedef BOOST_SIGNALS_NAMESPACE::detail::slot_connection<slot_function_type, 
      group_type, group_compare_type, threading_model_type, allocator_type> 
      slot_connection_type;

    // The function object passed to the slot call iterator that will call
    // the underlying slot function with its arguments bound
    typedef BOOST_SIGNALS_NAMESPACE::detail::BOOST_SIGNALS_CALL_BOUND<R>
      outer_bound_slot_caller;

    typedef typename outer_bound_slot_caller::template
              caller<BOOST_SIGNALS_TEMPLATE_ARGS
                     BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
                     slot_function_type,
                     slot_connection_type>
      call_bound_slot;

  public:
    // Slot call iterator type.
    typedef BOOST_SIGNALS_NAMESPACE::detail::slot_call_iterator<
      call_bound_slot, typename base_type::impl_base_type>
      slot_call_iterator;

    // Argument type enumeration (for boost::bind, etc.)
    BOOST_SIGNALS_ARG_TYPES

    // Argument types (for the standard library)
    BOOST_SIGNALS_STANDARD_ARG_TYPES

  public:
    // Public interface member functions.

    // Construction.
    explicit BOOST_SIGNALS_SIGNAL(const combiner_type& c = combiner_type(),
      const group_compare_type& g = group_compare_type())
      : base_type(c, g)
    { }

    // Destruction.
    ~BOOST_SIGNALS_SIGNAL()
    { }

    // Slot management: connect a slot with a group id.
    BOOST_SIGNALS_NAMESPACE::connection connect(
      const group_type& group, 
      const slot_type& slot_model,
      BOOST_SIGNALS_NAMESPACE::connect_position at = BOOST_SIGNALS_NAMESPACE::at_back
      );

    // Slot management: connect a slot without a group id.
    BOOST_SIGNALS_NAMESPACE::connection connect(
      const slot_type& slot_model,
      BOOST_SIGNALS_NAMESPACE::connect_position at = BOOST_SIGNALS_NAMESPACE::at_back
      );

    // Slot management: disconnect a slot by group id or target function.
    template<typename T>
    void disconnect(const T& t)
    {
      typedef mpl::bool_<(is_convertible<T, group_type>::value)> is_group;
      this->do_disconnect(t, is_group());
    }

    // Slot invocation: non-const version.
    result_type operator()(BOOST_SIGNALS_PARMS);

    // Slot invocation: const version.
    result_type operator()(BOOST_SIGNALS_PARMS) const;

    // Combiner access
    typename threading_model_type::get_combiner_type<combiner_type>::type 
      combiner()
    { 
      return impl_->combiner(); 
    }

    typename threading_model_type::get_combiner_const_type<combiner_type>::type 
      combiner() const
    { 
      return impl_->combiner(); 
    }

    void set_combiner(const combiner_type& c);

  private:
    // Disconnect a named slot.
    void do_disconnect(const group_type& group, mpl::bool_<true>)
    {
      // Compilation error? You're trying to disconnect slots by group name for
      // a no_name signal.
      BOOST_STATIC_ASSERT(!(is_same<group_type, 
        BOOST_SIGNALS_NAMESPACE::no_name>::value));
      impl_->disconnect(group);
    }

    // Disconnect slot by target function.
    template<typename Function>
    void do_disconnect(const Function& f, mpl::bool_<false>)
    {
      // Notify the slot handling code that we are iterating through the slots
      typename signal_impl_type::signal_lock lock(this->impl_);

      if(lock) {
        iterator i = impl_->slots_.begin();
        while(i != impl_->slots_.end()) {
          slot_connection_type* slot = 
            static_cast<slot_connection_type*>((*i++).get());
          if(slot->get_slot_function() == f)
            slot->disconnect();
        }
      }
    }
  };

  template<
    typename R,
    BOOST_SIGNALS_TEMPLATE_PARMS
    BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ThreadingModel,
    typename Allocator
  >
  typename BOOST_SIGNALS_SIGNAL<
             R, BOOST_SIGNALS_TEMPLATE_ARGS
             BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
             Combiner, Group, GroupCompare,
             SlotFunction, ThreadingModel, Allocator
             >::result_type
  BOOST_SIGNALS_SIGNAL<
    R, BOOST_SIGNALS_TEMPLATE_ARGS
    BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
    Combiner, Group, GroupCompare,
    SlotFunction, ThreadingModel, Allocator
  >::operator()(BOOST_SIGNALS_PARMS)
  {
    // Notify the slot handling code that we are making a call
    typename impl_type::signal_call_lock lock(this->impl_);

    // Construct a function object that will call the underlying slots
    // with the given arguments.
#if BOOST_SIGNALS_NUM_ARGS == 0
    BOOST_SIGNALS_ARGS_STRUCT_INST args;
#else
    BOOST_SIGNALS_ARGS_STRUCT_INST args(BOOST_SIGNALS_ARGS);
#endif // BOOST_SIGNALS_NUM_ARGS > 0
    call_bound_slot f(&args);

    typedef typename call_bound_slot::result_type result_type;
    optional<result_type> cache;
    combiner_type combiner = impl_->combiner();

    // Let the combiner call the slots via a pair of input iterators
    return combiner(slot_call_iterator(impl_->slots_.begin(),
                                       impl_->slots_.end(), f, cache),
                    slot_call_iterator(impl_->slots_.end(),
                                       impl_->slots_.end(), f, cache));
  }

  template<
    typename R,
    BOOST_SIGNALS_TEMPLATE_PARMS
    BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
    typename Combiner,
    typename Group,
    typename GroupCompare,
    typename SlotFunction,
    typename ThreadingModel,
    typename Allocator
  >
  typename BOOST_SIGNALS_SIGNAL<
             R, BOOST_SIGNALS_TEMPLATE_ARGS
             BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
             Combiner, Group, GroupCompare,
             SlotFunction, ThreadingModel, Allocator
             >::result_type
  BOOST_SIGNALS_SIGNAL<
    R, BOOST_SIGNALS_TEMPLATE_ARGS
    BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
    Combiner, Group, GroupCompare,
    SlotFunction, ThreadingModel, Allocator
  >::operator()(BOOST_SIGNALS_PARMS) const
  {
    // Notify the slot handling code that we are making a call
    typename impl_type::signal_call_lock lock(this->impl_);

    // Construct a function object that will call the underlying slots
    // with the given arguments.
#if BOOST_SIGNALS_NUM_ARGS == 0
    BOOST_SIGNALS_ARGS_STRUCT_INST args;
#else
    BOOST_SIGNALS_ARGS_STRUCT_INST args(BOOST_SIGNALS_ARGS);
#endif // BOOST_SIGNALS_NUM_ARGS > 0
    call_bound_slot f(&args);

    typedef typename call_bound_slot::result_type result_type;
    optional<result_type> cache;
    combiner_type combiner = impl_->combiner();

    // Let the combiner call the slots via a pair of input iterators
    return combiner(slot_call_iterator(impl_->slots_.begin(),
                                       impl_->slots_.end(), f, cache),
                    slot_call_iterator(impl_->slots_.end(),
                                       impl_->slots_.end(), f, cache));
  }

#ifndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      template<int Arity,
               typename Signature,
               typename Combiner,
               typename Group,
               typename GroupCompare,
               typename SlotFunction,
               typename ThreadingModel,
               typename Allocator
      >
      class real_get_signal_impl;

      template<typename Signature,
               typename Combiner,
               typename Group,
               typename GroupCompare,
               typename SlotFunction,
               typename ThreadingModel,
               typename Allocator
      >
      class real_get_signal_impl<BOOST_SIGNALS_NUM_ARGS, Signature, Combiner, 
                                 Group, GroupCompare, SlotFunction,
                                 ThreadingModel, Allocator>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef BOOST_SIGNALS_SIGNAL<typename traits::result_type,
                        BOOST_SIGNALS_TRAITS_ARG_TYPES
                        BOOST_SIGNALS_COMMA_IF_NONZERO_ARGS
                        Combiner,
                        Group,
                        GroupCompare,
                        SlotFunction,
                        ThreadingModel,
                        Allocator
                  > type;
      };
    } // namespace detail
  } // namespace BOOST_SIGNALS_NAMESPACE
#endif // ndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX
} // namespace boost

// Macro cleanup
#include <boost/signals/detail/epilogue.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif
