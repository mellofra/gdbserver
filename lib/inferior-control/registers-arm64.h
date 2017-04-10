// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

namespace debugserver {
namespace arch {

// The arm64 general register names.

enum class Arm64Register {
  X0 = 0,
  FP = 29,
  SP = 31,
  PC = 32,
  CPSR = 33,
  NUM_REGISTERS
};

}  // namespace arch
}  // namespace debugserver
