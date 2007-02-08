// Boost.Signals library

// Copyright Timmo Stange 2007. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SLOT_SIGNAL_INTERFACE_HPP
#define BOOST_SIGNALS_DETAIL_SLOT_SIGNAL_INTERFACE_HPP

#include <boost/signals/detail/slot_connection_interface.hpp>
#include <boost/shared_ptr.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    namespace detail {
      // slot_signal_interface class template.
      // Provides slot state manipulation and observation for parts of the
      // library that are aware of the signal implementation in use.
      template<class SignalImpl, class BoundObjectsTracker>
      class slot_signal_interface 
        : public slot_connection_interface, public BoundObjectsTracker
      {
        typedef typename SignalImpl::slot_iterator iterator;
        typedef typename SignalImpl::signal_lock signal_lock;
      public:
        // Acquire the slot from a locked signal context (protect it from being 
        // removed). This and release() below is used to ensure a valid iterator
        // during signal invocation.
        void acquire() const
        {
          if(ref_count_ != 0)
            ++ref_count_;
        }
        // Release the slot from a locked signal context (remove if count drops to zero).
        void release() const
        {
          if(ref_count_ != 0) {
            if(--ref_count_ == 0) {
#ifndef BOOST_SIGNALS_NO_LEGACY_SUPPORT
              // The legacy tracker may call us back during destruction.
              stop_tracking();
#endif
              shared_ptr<SignalImpl> sig(signal_);
              signal_.reset();
              sig->remove_slot(iter_);
              // We might be dead from here!
            }
          }
        }

      protected:
        slot_signal_interface(const shared_ptr& sig, const iterator pos)
          : slot_connection_interface(&set_slot_state, &get_slot_state),
#ifndef BOOST_SIGNALS_NO_LEGACY_SUPPORT
          // The legacy tracker needs access to disconnect slots.
          BoundObjectsTracker(this),
#endif
          signal_(sig), iter_(pos), block_(0), ref_count_(1), 
          disconnected_(false)
        { }

        ~slot_signal_interface() {
          // If we haven't been disconnected properly then something's
          // rotten in the state of Denmark.
          assert(ref_count_ == 0);
          assert(!signal_);
        }
      private:
        // Change the slot's state.
        void set_state(bool* connected, bool* blocked) 
        {
          if(signal_) {
            signal_lock lock(signal_);
            // Adjust the blocking state, if requested.
            if(blocked) {
              block_ = *blocked ? 1 : 0;
            }
            if(connected) {
              if(!*connected) {
                if(!disconnected_) {
                  disconnected_ = true;
                  release();
                }
              }
            }
          }
        }

        // Retrieve the slot's state.
        void get_state(bool* connected, bool* blocked) const
        {
          if(signal_) {
            signal_lock lock(signal_);
            // Adjust the blocking state, if requested.
            if(blocked) {
              *blocked = (block_ != 0);
            }
            if(connected) {
              if(!disconnected_)
                *connected = check_tracked_objects();
              else
                *connected = false;
            }
          } else {
            // The caller holds a shared_ptr to a dead slot.
            if(connected) *connected = false;
            if(blocked) *blocked = false;
          }
        }

        // Static to-member forwarding for set and get.
        static void set_slot_state(slot_connection_interface* p, 
                                    bool* c, bool* b)
        {
          static_cast<slot_signal_interface*>(p)->set_state(c, b);
        }

        static void get_slot_state(const slot_connection_interface* p, 
                                    bool* c, bool* b)
        {
          static_cast<const slot_signal_interface*>(p)->get_state(c, b);
        }

      private:
        shared_ptr<SignalImpl> signal_;
        iterator iter_;
        int ref_count_;
        int block_;
        bool disconnected_;
      };
    } // end namespace detail
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_SLOT_SIGNAL_INTERFACE_HPP
