// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_SLOT_SIGNAL_INTERFACE_HPP
#define BOOST_SIGNALS_DETAIL_SLOT_SIGNAL_INTERFACE_HPP

#include <boost/signals/detail/slot_tracking_base.hpp>
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
      template<class SignalImpl>
      class slot_signal_interface 
        : public SignalImpl::slot_tracking_base_type
      {
        typedef typename SignalImpl::slot_iterator iterator;
        typedef typename SignalImpl::signal_lock signal_lock;
        typedef typename SignalImpl::slot_tracking_base_type base_type;
      public:
        // Acquire the slot from a locked signal context (protect it from being 
        // removed). This and release() below is used to ensure a valid iterator
        // during signal invocation.
        void acquire()
        {
          if(ref_count_ != 0)
            ++ref_count_;
        }
        // Release the slot from a locked signal context (remove if count drops to zero).
        void release()
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

        // Release if not already disconnected.
        void disconnect() 
        {
          if(!disconnected())
            release();
        }

        // Check the connection state from a locked signal context.
        bool disconnected() const 
        {
          return disconnected_;
        }
        // Check the blocking state from a locked signal context.
        bool blocked() const 
        {
          return block_ != 0;
        }
        // Check the callability from a locked signal context.
        bool callable() const 
        {
          return !disconnected_ && (block_ == 0);
        }

        // Initialize the slot-signal connection.
        void reset(const shared_ptr<SignalImpl>& sig, const iterator pos)
        {
          signal_ = sig;
          iter_ = pos;
          disconnected_ = !sig;
          ref_count_ = disconnected_ ? 0 : 1;
        }

      protected:
        slot_signal_interface()
          : base_type(&set_slot_state, &get_slot_state),
          signal_(), iter_(), block_(0), ref_count_(0), 
          disconnected_(true)
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
          signal_lock lock(signal_);
          if(lock) {
            // Adjust the blocking state, if requested.
            if(blocked) {
              block_ = *blocked ? 1 : 0;
            }
            // Disconnect the slot, if requested.
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
            if(connected) {
              // The caller wants the connection state.
              if(!disconnected_) {
                // We only need a lock to check the tracked objects - for
                // all other purposes the reliability of the returned
                // "snapshot" depends on external synchronization.
                signal_lock lock(signal_);
                if(lock) {
                  *connected = check_tracked_objects();
                } else {
                  *connected = false;
                }
              } else {
                *connected = false;
              }
            }
            if(blocked) {
              // The caller wants the blocking state.
              *blocked = (block_ != 0);
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
