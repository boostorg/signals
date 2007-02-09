// Boost.Signals library

// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS_DETAIL_THREADING_MODEL_HEADER
#define BOOST_SIGNALS_DETAIL_THREADING_MODEL_HEADER

#include <boost/thread/recursive_mutex.hpp>
#include <boost/smart_ptr.hpp>

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_PREFIX
#endif

namespace boost {
  namespace BOOST_SIGNALS_NAMESPACE {
    class single_threaded
    {
    public:
      // A lock securing exclusive access to a signal.
      // Does nothing but checking if the signal exists in this threading
      // model.
      struct signal_lock
      {
        signal_lock(const shared_ptr<single_threaded>& sp)
          : exists_(true)
        { }

        signal_lock(const weak_ptr<single_threaded>& wp)
          : exists_(!wp.expired())
        { }

        typedef bool signal_lock::*unspecified_bool_type;
        operator unspecified_bool_type() const {
          return exists_ ? &signal_lock::exists_ : 0;
        }

        bool exists_;
      };

      // A lock securing exclusive access to and lifetime of a signal.
      struct signal_call_lock
      {
        signal_call_lock(const shared_ptr<single_threaded>& sp)
          : ptr_(sp)
        { }

        signal_call_lock(const weak_ptr<single_threaded>& wp)
          : ptr_(wp.lock())
        { }

        typedef const shared_ptr<single_threaded> 
          signal_call_lock::*unspecified_bool_type;

        operator unspecified_bool_type() const {
          return ptr_ ? &signal_call_lock::ptr_ : 0;
        }

        const shared_ptr<single_threaded> ptr_;
      };

      // A local lock to a signal.
      // Does nothing in this threading model.
      struct local_lock
      {
        local_lock(void*)
        { }
      };
    };

    class multi_threaded : private recursive_mutex
    {
    public:
      multi_threaded()
        : recursive_mutex(), lock_(*this, false)
      { }

      // A lock securing exclusive access to and lifetime of a signal.
      struct signal_lock
      {
        signal_lock(const shared_ptr<multi_threaded>& sp)
          : ptr_(sp)
        {
          if(ptr_) ptr_->lock_mutex();
        }

        signal_lock(const weak_ptr<multi_threaded>& wp)
          : ptr_(wp.lock())
        {
          if(ptr_) ptr_->lock_mutex();
        }

        ~signal_lock()
        {
          if(ptr_) ptr_->unlock_mutex();
        }

        typedef const shared_ptr<multi_threaded> 
          signal_lock::*unspecified_bool_type;

        operator unspecified_bool_type() const {
          return ptr_ ? &signal_lock::ptr_ : 0;
        }

        const shared_ptr<multi_threaded> ptr_;
      };

      typedef signal_lock signal_call_lock;

      // A local lock to a signal.
      struct local_lock
      {
        local_lock(const multi_threaded* p)
          : ptr_(p)
        {
          ptr_->lock_mutex();
        }
        ~local_lock()
        {
          ptr_->unlock_mutex();
        }
        const multi_threaded* ptr_;
      };

    private:
      friend struct signal_lock;
      friend struct local_lock;

      void lock_mutex() const {
        lock_.lock();
      }
      void unlock_mutex() const {
        lock_.unlock();
      }

      mutable recursive_mutex::scoped_lock lock_;
    };
  } // end namespace BOOST_SIGNALS_NAMESPACE
} // end namespace boost

#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif

#endif // BOOST_SIGNALS_DETAIL_THREADING_MODEL_HEADER
