// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_SLOT_HEADER
#define BOOST_SIGNALS_SLOT_HEADER

#include <boost/signals/detail/signals_common.hpp>
#include <boost/signals/single_threaded.hpp>
#include <boost/signals/detail/slot_signal_interface.hpp>
#include <boost/signals/detail/signal_impl_base.hpp>
#include <boost/signals/detail/slot_tracking_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  template<typename F>
  class slot;

  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // slot_connection class template.
      // This represents an actual callable slot stored in a signal and refered
      // to by client connections.
      template<typename SlotFunction,
        typename Group,
        typename GroupCompare,
        typename ThreadingModel,
        typename Allocator
      >
      class slot_connection 
        : public slot_signal_interface<signal_impl_base<Group, 
                                                        GroupCompare, 
                                                        ThreadingModel,
                                                        Allocator> > 
      {
        typedef slot_signal_interface<signal_impl_base<Group, 
                                                        GroupCompare, 
                                                        ThreadingModel,
                                                        Allocator> > base_type;
      public:
        slot_connection(const SlotFunction& f, 
          const typename base_type::trackable_objects_container& bo)
          : slot_function_(f)
        {
          // TODO: Could optimize the vector size with a reserve() here.
          trackable_objects_ = bo;
          start_tracking();
        }

        // Get the slot function to call the actual slot
        const SlotFunction& get_slot_function() const { return slot_function_; }

      private:
        SlotFunction slot_function_;
      };

      // slot_friend structure.
      // Provides a loophole for signals to access a slot's private members.
      // (We can't make all the possible signal instantiations friends).
      struct slot_friend
      {
        template<typename F, class G, class GC, class TM, class A>
        static shared_ptr<slot_connection<F,G,GC,TM,A> > 
          create_slot(const slot<F>& slot) {
            return slot.create_slot_connection<slot_connection<F,G,GC,TM,A> >();
        }
      
        template<typename F>
        static bool is_active(const slot<F>& slot) {
            return slot.is_active();
        }
      };
    }
    // Get the slot so that it can be copied
    template<typename F>
    reference_wrapper<const F>
    get_invocable_slot(const F& f, BOOST_SIGNALS_NAMESPACE::detail::signal_tag)
      { return reference_wrapper<const F>(f); }

    template<typename F>
    const F&
    get_invocable_slot(const F& f, BOOST_SIGNALS_NAMESPACE::detail::reference_tag)
      { return f; }

    template<typename F>
    const F&
    get_invocable_slot(const F& f, BOOST_SIGNALS_NAMESPACE::detail::value_tag)
      { return f; }

    // Get the slot so that it can be inspected for trackable objects
    template<typename F>
    const F&
    get_inspectable_slot(const F& f, BOOST_SIGNALS_NAMESPACE::detail::signal_tag)
      { return f; }

    template<typename F>
    const F&
    get_inspectable_slot(const reference_wrapper<F>& f, BOOST_SIGNALS_NAMESPACE::detail::reference_tag)
      { return f.get(); }

    template<typename F>
    const F&
    get_inspectable_slot(const F& f, BOOST_SIGNALS_NAMESPACE::detail::value_tag)
      { return f; }

    // Determines the type of the slot - is it a signal, a reference to a
    // slot or just a normal slot.
    template<typename F>
    typename BOOST_SIGNALS_NAMESPACE::detail::get_slot_tag<F>::type
    tag_type(const F&)
    {
      typedef typename BOOST_SIGNALS_NAMESPACE::detail::get_slot_tag<F>::type
        the_tag_type;
      the_tag_type tag = the_tag_type();
      return tag;
    }
  } // end namespace BOOST_SIGNALS_NAMESPACE

  // slot class template.
  // This represents a model for a slot and can be created by the client.
  template<typename SlotFunction>
  class slot 
    : private BOOST_SIGNALS_NAMESPACE::detail::dual_slot_tracking_base
  {
    typedef BOOST_SIGNALS_NAMESPACE::detail::dual_slot_tracking_base base_type;

  public:
    template<typename F>
    slot(const F& f) 
      : base_type(&set_slot_state, &get_slot_state),
      active_(true),
      slot_function_(
        BOOST_SIGNALS_NAMESPACE::get_invocable_slot(f, BOOST_SIGNALS_NAMESPACE::tag_type(f))
      )
    {
      setup_tracking(BOOST_SIGNALS_NAMESPACE::get_inspectable_slot(f, 
        BOOST_SIGNALS_NAMESPACE::tag_type(f)));
      start_tracking();
      active_ = check_tracked_objects();
    }

#ifdef __BORLANDC__
    template<typename F>
    slot(F* f) : slot_function(f)
    {
    }
#endif // __BORLANDC__

  private:
    friend BOOST_SIGNALS_NAMESPACE::detail::slot_friend;

    template<typename T>
    shared_ptr<T> create_slot_connection() const
    {
      return shared_ptr<T>(
          new T(slot_function_, trackable_objects_)
        );
    }

    bool is_active() const {
      return active_;
    }

    // Static to-member forwarding for set and get.
    static void set_slot_state(
      BOOST_SIGNALS_NAMESPACE::detail::slot_connection_interface* p, 
      bool* c, bool*)
    {
      if(c) {
        static_cast<slot*>(p)->active_ = *c;
      }
    }

    static void get_slot_state(
      const BOOST_SIGNALS_NAMESPACE::detail::slot_connection_interface* p, 
      bool* c, bool*)
    {
      if(c) {
        *c = static_cast<const slot*>(p)->active_;
      }
    }

    slot(); // no default constructor
    slot& operator=(const slot&); // no assignment operator

    SlotFunction slot_function_;
    bool active_;
  };
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SLOT_HEADER
