// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <magenta/types.h>

#include "lib/ftl/macros.h"

namespace debugserver {
namespace util {

struct LoadMap {
  mx_koid_t pid;
  uint64_t base_addr;
  uint64_t load_addr;
  uint64_t end_addr;
  std::string name;
  std::string so_name;
  std::string build_id;
};

class LoadMapTable {
 public:
  LoadMapTable() = default;

  bool ReadLogListenerOutput(const std::string& file);

  const LoadMap* LookupLoadMap(mx_koid_t pid, uint64_t addr);

 private:
  void Clear();
  void AddLoadMap(const LoadMap& map);

  std::vector<LoadMap> maps_;

  FTL_DISALLOW_COPY_AND_ASSIGN(LoadMapTable);
};

}  // namespace util
}  // namespace debugserver
