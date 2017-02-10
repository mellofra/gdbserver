// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "io-loop.h"

#include <array>

namespace debugserver {

// This class implements IOLoop for Remote Serial Protocol support.

class RspIOLoop final : public IOLoop {
 public:
  RspIOLoop(int in_fd, Delegate* delegate);

 private:
  // Maximum number of characters in the inbound buffer.
  constexpr static size_t kMaxBufferSize = 4096;

  void OnReadTask() override;

  // Buffer used for reading incoming bytes.
  std::array<char, kMaxBufferSize> in_buffer_;

  FTL_DISALLOW_COPY_AND_ASSIGN(RspIOLoop);
};

}  // namespace debugserver
