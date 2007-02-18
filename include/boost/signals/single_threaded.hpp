// Boost.Signals library

// Copyright Timmo Stange 2007. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_SINGLE_THREADED_HEADER
#define BOOST_SIGNALS_SINGLE_THREADED_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/slot_tracking_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {

      // single_threaded_impl_base class.
      class single_threaded_impl_base : noncopyable
      { };

      // null_signal_lock class.
      class null_signal_lock
      {
      public:
        template<typename T>
        null_signal_lock(const weak_ptr<T>& p)
          : exists_(!p.expired())
        { }

        typedef bool null_signal_lock::*unspecified_bool_type;

        operator unspecified_bool_type() const 
        {
          return exists_ ? &null_signal_lock::exists_ : 0;
        }

        bool exists_;
      };

      // simple_signal_call_lock class.
      class simple_signal_call_lock
      {
      public:
        template<typename T>
        simple_signal_call_lock(const weak_ptr<T>& p)
          : ptr_(p.lock())
        { }
        template<typename T>
        simple_signal_call_lock(const shared_ptr<T>& p)
          : ptr_(p)
        { }

        typedef const shared_ptr<single_threaded_impl_base> 
          simple_signal_call_lock::*unspecified_bool_type;

        operator unspecified_bool_type() const 
        {
          return ptr_ ? &simple_signal_call_lock::ptr_ : 0;
        }

        const shared_ptr<single_threaded_impl_base> ptr_;
      };

      // null_local_lock class.
      class null_local_lock
      {
      public:
        null_local_lock(single_threaded_impl_base*)
        { }
      };

    } // end namespace detail

    // single_threaded policy class.
    // Provides the necessary typedefs for single threaded signal operations.
    class single_threaded
    {
    public:
      // Base type for signal implementations of this threading model.
      typedef detail::single_threaded_impl_base signal_impl_threading_base_type;

      // Tracking base type for slot implementations with this threading model.
      typedef detail::dual_slot_tracking_base slot_tracking_base_type;

      // Return types for combiner access.
      template<typename Combiner>
      struct get_combiner_type {
        typedef Combiner& type;
      };

      template<typename Combiner>
      struct get_combiner_const_type {
        typedef const Combiner& type;
      };

      // Lock class to synchronize access to a signal implementation.
      typedef detail::null_signal_lock signal_lock;

      // Lock class to synchronize access to and ensure the lifetime of
      // a signal implementation.
      typedef detail::simple_signal_call_lock signal_call_lock;

      // Lock class for use from within the signal implementation.
      typedef detail::null_local_lock local_lock;
    };
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SINGLE_THREADED_HEADER
