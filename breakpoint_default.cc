// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "breakpoint.h"

#include "lib/ftl/logging.h"

namespace debugserver {
namespace arch {

bool SoftwareBreakpoint::Insert() {
  return false;
}

bool SoftwareBreakpoint::Remove() {
  return false;
}

bool SoftwareBreakpoint::IsInserted() const {
  return false;
}

bool SingleStepBreakpoint::Insert() {
  return false;
}

bool SingleStepBreakpoint::Remove() {
  return false;
}

bool SingleStepBreakpoint::IsInserted() const {
  return false;
}

}  // namespace arch
}  // namespace debugserver
