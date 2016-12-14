// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "thread.h"

#include <cinttypes>
#include <string>

#include <magenta/syscalls.h>
#include <magenta/syscalls/exception.h>

#include "lib/ftl/logging.h"
#include "lib/ftl/strings/string_printf.h"

#include "arch.h"
#include "process.h"
#include "util.h"

namespace debugserver {

// static
const char* Thread::StateName(Thread::State state) {
#define CASE_TO_STR(x) \
  case x:              \
    return #x
  switch (state) {
    CASE_TO_STR(Thread::State::kNew);
    CASE_TO_STR(Thread::State::kGone);
    CASE_TO_STR(Thread::State::kStopped);
    CASE_TO_STR(Thread::State::kRunning);
    default:
      break;
  }
#undef CASE_TO_STR
  return "(unknown)";
}

Thread::Thread(Process* process, mx_handle_t debug_handle, mx_koid_t id)
    : process_(process),
      debug_handle_(debug_handle),
      id_(id),
      state_(State::kNew),
      breakpoints_(this),
      weak_ptr_factory_(this) {
  FTL_DCHECK(process_);
  FTL_DCHECK(debug_handle_ != MX_HANDLE_INVALID);
  FTL_DCHECK(id_ != MX_KOID_INVALID);

  registers_ = arch::Registers::Create(this);
  FTL_DCHECK(registers_.get());
}

Thread::~Thread() {
  FTL_VLOG(2) << "Destructing thread " << GetDebugName();
  // We don't use the mx classes so we must manually close this handle.
  if (debug_handle_ != MX_HANDLE_INVALID)
    mx_handle_close(debug_handle_);
}

std::string Thread::GetName() const {
  return ftl::StringPrintf("%" PRId64 ".%" PRId64, process_->id(), id());
}

std::string Thread::GetDebugName() const {
  return ftl::StringPrintf("%" PRId64 ".%" PRId64 "(%" PRIx64 ".%" PRIx64 ")",
                           process_->id(), id(), process_->id(), id());
}

void Thread::set_state(State state) {
  FTL_DCHECK(state != State::kNew);
  state_ = state;
}

void Thread::FinishExit() {
  // We close the handle here so the o/s will release the thread.
  if (debug_handle_ != MX_HANDLE_INVALID)
    mx_handle_close(debug_handle_);
  debug_handle_ = MX_HANDLE_INVALID;
}

ftl::WeakPtr<Thread> Thread::AsWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

int Thread::GetGdbSignal() const {
  if (!exception_context_)
    return -1;

  return arch::ComputeGdbSignal(*exception_context_);
}

void Thread::SetExceptionContext(const mx_exception_context_t& context) {
  exception_context_.reset(new mx_exception_context_t);
  *exception_context_ = context;
}

bool Thread::Resume() {
  if (state() != State::kStopped && state() != State::kNew) {
    FTL_LOG(ERROR) << "Cannot resume a thread while in state: "
                   << StateName(state());
    return false;
  }

  // This is printed here before resuming the task so that this is always
  // printed before any subsequent exception report (which is read by another
  // thread).
  FTL_VLOG(2) << "Thread " << GetName() << " is now running";

  mx_status_t status = mx_task_resume(debug_handle_, MX_RESUME_EXCEPTION);
  if (status < 0) {
    util::LogErrorWithMxStatus("Failed to resume thread", status);
    return false;
  }

  state_ = State::kRunning;
  FTL_LOG(INFO) << "Thread (tid = " << id_ << ") is running";

  return true;
}

}  // namespace debugserver
