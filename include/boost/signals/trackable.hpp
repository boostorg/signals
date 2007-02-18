// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_TRACKABLE_HPP
#define BOOST_SIGNALS_TRACKABLE_HPP

#include <boost/signals/detail/config.hpp>
#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/detail/slot_connection_interface.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/ref.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/mpl/bool.hpp>
#include <list>
#include <vector>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {

namespace BOOST_SIGNALS_NAMESPACE {
  namespace detail {
    // dual_slot_tracking_base forward declaration for friend declaration.
    class dual_slot_tracking_base;
  }
  // Base class for "trackable" objects that can be tracked when they are
  // bound in slot target functions. When a trackable object is destroyed,
  // the signal/slot connections are disconnected automatically.
  class BOOST_SIGNALS_DECL trackable {
  protected:
    trackable() 
      : connected_slots_(), dying_(false) 
    { }
    trackable(const trackable&) 
      : connected_slots_(), dying_(false) 
    { }
    ~trackable();

    trackable& operator=(const trackable&)
    {
      connected_slots_.clear();
      return *this;
    }

  private:
    friend class detail::dual_slot_tracking_base;

    void add_slot(detail::slot_connection_interface*) const;
    void remove_slot(const detail::slot_connection_interface*) const;

    typedef std::list<detail::slot_connection_interface*> slot_list;
    typedef slot_list::iterator slot_iterator;

    // List of connections that this object is part of
    mutable slot_list connected_slots_;

    // True when the object is being destroyed
    mutable bool dying_;
  };

  namespace detail {
    // A visitor that adds each trackable object to a vector
    class trackable_objects_visitor {
    public:
      trackable_objects_visitor(std::vector<const trackable*>& v) :
        trackable_objects_(v)
      {
      }

      template<typename T>
      void operator()(const T& t) const
      {
        decode(t, 0);
      }

    private:
      // decode() decides between a reference wrapper and anything else
      template<typename T>
      void decode(const reference_wrapper<T>& t, int) const
      {
        add_if_trackable(t.get_pointer());
      }

      template<typename T>
      void decode(const T& t, long) const
      {
        typedef mpl::bool_<(is_pointer<T>::value)> is_a_pointer;
        maybe_get_pointer(t, is_a_pointer());
      }

      // maybe_get_pointer() decides between a pointer and a non-pointer
      template<typename T>
      void maybe_get_pointer(const T& t, mpl::bool_<true>) const
      {
        add_if_trackable(t);
      }

      template<typename T>
      void maybe_get_pointer(const T& t, mpl::bool_<false>) const
      {
        // Take the address of this object, because the object itself may be
        // trackable
        add_if_trackable(boost::addressof(t));
      }

      // add_if_trackable() adds trackable objects to the list of bound objects
      inline void add_if_trackable(const trackable* b) const
      {
        if (b) {
          trackable_objects_.push_back(b);
        }
      }

      inline void add_if_trackable(const void*) const { }

      template<typename R>
      inline void add_if_trackable(R (*)()) const { }

      template<typename R, typename T1>
      inline void add_if_trackable(R (*)(T1)) const { }

      template<typename R, typename T1, typename T2>
      inline void add_if_trackable(R (*)(T1, T2)) const { }

      template<typename R, typename T1, typename T2, typename T3>
      inline void add_if_trackable(R (*)(T1, T2, T3)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4>
      inline void add_if_trackable(R (*)(T1, T2, T3, T4)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5>
      inline void add_if_trackable(R (*)(T1, T2, T3, T4, T5)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6>
      inline void add_if_trackable(R (*)(T1, T2, T3, T4, T5, T6)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6, typename T7>
      inline void add_if_trackable(R (*)(T1, T2, T3, T4, T5, T6, T7)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6, typename T7, typename T8>
      inline void 
      add_if_trackable(R (*)(T1, T2, T3, T4, T5, T6, T7, T8)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6, typename T7, typename T8, typename T9>
      inline void
      add_if_trackable(R (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9)) const { }

      template<typename R, typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6, typename T7, typename T8, typename T9,
               typename T10>
      inline void
      add_if_trackable(R (*)(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)) const { }

      std::vector<const trackable*>& trackable_objects_;
    };
  } // end namespace detail
} // end namespace BOOST_SIGNALS_NAMESPACE

} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_TRACKABLE_HPP
