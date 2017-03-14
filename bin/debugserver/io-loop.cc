// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "io-loop.h"

#include <unistd.h>

#include "debugger-utils/util.h"

#include "lib/ftl/logging.h"
#include "lib/mtl/tasks/message_loop.h"

namespace debugserver {

RspIOLoop::RspIOLoop(int in_fd, Delegate* delegate)
    : IOLoop(in_fd, delegate) {
}

void RspIOLoop::OnReadTask() {
  FTL_DCHECK(mtl::MessageLoop::GetCurrent()->task_runner().get() ==
             read_task_runner().get());

  ssize_t read_size = read(fd(), in_buffer_.data(), kMaxBufferSize);

  // 0 bytes means that the remote end closed the TCP connection.
  if (read_size == 0) {
    FTL_VLOG(1) << "Client closed connection";
    ReportDisconnected();
    return;
  }

  // There was an error
  if (read_size < 0) {
    util::LogErrorWithErrno("Error occurred while waiting for a packet");
    ReportError();
    return;
  }

  ftl::StringView bytes_read(in_buffer_.data(), read_size);
  FTL_VLOG(2) << "-> " << util::EscapeNonPrintableString(bytes_read);

  // Notify the delegate that we read some bytes. We copy the buffer data
  // into the closure as |in_buffer_| can get modified before the closure
  // runs.
  // TODO(armansito): Pass a weakptr to |delegate_|?
  origin_task_runner()->PostTask([ bytes_read = bytes_read.ToString(), this ] {
    delegate()->OnBytesRead(bytes_read);
  });

  if (!quit_called())
    read_task_runner()->PostTask(std::bind(&RspIOLoop::OnReadTask, this));
}

}  // namespace debugserver
