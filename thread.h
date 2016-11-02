// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>

#include <magenta/syscalls/exception.h>
#include <magenta/syscalls/types.h>
#include <magenta/types.h>

#include "lib/ftl/macros.h"
#include "lib/ftl/memory/weak_ptr.h"

#include "registers.h"

namespace debugserver {

class Process;

// Represents a thread that is owned by a Process instance.
class Thread final {
 public:
  enum class State {
    kNew,
    kGone,
    kStopped,
    kRunning
  };

  Thread(Process* process, mx_handle_t debug_handle, mx_koid_t thread_id);
  ~Thread();

  mx_handle_t debug_handle() const { return debug_handle_; }
  mx_koid_t thread_id() const { return thread_id_; }

  // Returns a weak pointer to this Thread instance.
  ftl::WeakPtr<Thread> AsWeakPtr();

  // Returns a pointer to the arch::Registers object associated with this
  // thread. The returned pointer is owned by this Thread instance and should
  // not be deleted by the caller.
  arch::Registers* registers() const { return registers_.get(); }

  // Returns the current state of this thread.
  State state() const { return state_; }

  // Returns a GDB signal number based on the current exception context. If no
  // exception context was set on this Thread or if the exception data from the
  // context does not map to a meaningful GDB signal number, this method returns
  // -1.
  int GetGdbSignal() const;

  // Sets the current exception context for this thread.
  void SetExceptionContext(const mx_exception_context_t& context);

  // Resumes the thread from a "stopped in exception" state. Returns true on
  // success, false on failure.
  bool Resume();

 private:
  friend class Process;

  // Called by Process to set the state of its threads.
  void set_state(State state) { state_ = state; }

  // The owning process.
  Process* process_;  // weak

  // The debug-capable handle that we use to invoke mx_debug_* syscalls.
  mx_handle_t debug_handle_;

  // The thread ID (also the kernel object ID) associated with this thread.
  mx_koid_t thread_id_;

  // The arch::Registers object associated with this thread.
  std::unique_ptr<arch::Registers> registers_;

  // The current state of the this thread.
  State state_;

  // Pointer to the most recent exception context that this Thread received via
  // an architectural exception. Contains nullptr if the thread never received
  // an exception.
  std::unique_ptr<mx_exception_context_t> exception_context_;

  // Note: This should remain the last member so it'll be destroyed and
  // invalidate its weak pointers before any other members are destroyed.
  ftl::WeakPtrFactory<Thread> weak_ptr_factory_;

  FTL_DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace debugserver
