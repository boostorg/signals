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
#include <boost/signals/detail/threading_model.hpp>
#include <boost/signals/detail/slot_signal_interface.hpp>
#ifdef BOOST_SIGNALS_NO_LEGACY_SUPPORT
#  define BOOST_SIGNALS_DEFAULT_MODEL boost::BOOST_SIGNALS_NAMESPACE::single_threaded
#else
#include <boost/signals/detail/legacy/legacy_implementation.hpp>
#  define BOOST_SIGNALS_DEFAULT_MODEL boost::BOOST_SIGNALS_NAMESPACE::detail::legacy_implementation
#endif // def BOOST_SIGNALS_NO_LEGACY_SUPPORT
#include <boost/signals/detail/signal_impl_base.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
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
  template<typename SlotFunction, class ThreadingModel = BOOST_SIGNALS_DEFAULT_MODEL>
  class slot 
    : private BOOST_SIGNALS_NAMESPACE::detail::slot_signal_interface<
                BOOST_SIGNALS_NAMESPACE::detail::signal_impl_base<ThreadingModel> 
              > 
  {
    typedef BOOST_SIGNALS_NAMESPACE::detail::slot_signal_interface<
              BOOST_SIGNALS_NAMESPACE::detail::signal_impl_base<ThreadingModel> 
            > base_type;

  public:
    template<typename F>
    slot(const F& f) 
      : slot_function(
        BOOST_SIGNALS_NAMESPACE::get_invocable_slot(f, BOOST_SIGNALS_NAMESPACE::tag_type(f))
        )
    {
      start_tracking(BOOST_SIGNALS_NAMESPACE::get_inspectable_slot(f, 
        BOOST_SIGNALS_NAMESPACE::tag_type(f)));
    }

#ifdef __BORLANDC__
    template<typename F>
    slot(F* f) : slot_function(f)
    {
    }
#endif // __BORLANDC__

    // We would have to enumerate all of the signalN classes here as friends
    // to make this private (as it otherwise should be). We can't name all of
    // them because we don't know how many there are.
  public:
    // Get the slot function to call the actual slot
    const SlotFunction& get_slot_function() const { return slot_function; }

    void release() const { stop_tracking(); }

  private:
    slot(); // no default constructor
    slot& operator=(const slot&); // no assignment operator

    SlotFunction slot_function;
  };
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SLOT_HEADER
