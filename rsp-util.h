// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <cstdint>
#include <cstdio>
#include <deque>
#include <string>
#include <vector>

#include <magenta/syscalls/exception.h>
#include <magenta/types.h>

#include "lib/ftl/strings/string_view.h"

namespace debugserver {
namespace util {

// The escape character used in the GDB Remote Protocol.
constexpr char kEscapeChar = '}';

// Potential Errno values used by GDB (see
// https://sourceware.org/gdb/onlinedocs/gdb/Errno-Values.html#Errno-Valuesfor
// reference). We don't rely on macros from errno.h because some of the integer
// definitions don't match.
// TODO(armansito): The error code definitions from GDB aren't really granular
// enough to aid debug various error conditions that packet handling potentially
// produces. Since GDB always ignores the error codes, perhaps we should come up
// with some of our own.
enum class ErrorCode {
  PERM = 1,
  NOENT = 2,
  INTR = 4,
  BADF = 9,
  ACCES = 13,
  FAULT = 14,
  BUSY = 16,
  EXIST = 17,
  NODEV = 19,
  NOTDIR = 20,
  ISDIR = 21,
  INVAL = 22,
  NFILE = 23,
  MFILE = 24,
  FBIG = 27,
  NOSPC = 28,
  SPIPE = 29,
  ROFS = 30,
  NAMETOOLONG = 91,
  UNKNOWN = 9999
};

// Builds an error response packet based on |error_code|. For example, if
// |error_code| is EPERM then the return value is "E01".
std::string BuildErrorPacket(ErrorCode error_code);

// Parses a thread ID (and optionally a process ID). Returns true if the given
// expression is parsed successfully and returns the process and thread IDs in
// |out_pid| and |out_tid|. If a process ID is present, then the value of
// |out_has_pid| is set to true, and to false otherwise.
//
// Note that we are not using mx_koid_t here because it is defined as uint64_t
// and the GDB remote protocol allows a value of "-1" to refer to "all"
// processes/threads. So we do our best and use int64_t.
//
// Returns false if the values cannot be parsed or if they cannot be represented
// as an int64_t.
//
// (See
// https://sourceware.org/gdb/current/onlinedocs/gdb/Packets.html#thread%2did%20syntax
// for reference).
bool ParseThreadId(const ftl::StringView& bytes,
                   bool* out_has_pid,
                   int64_t* out_pid,
                   int64_t* out_tid);

// Encodes the given thread and process IDs using the GDB remote protocol thread
// ID syntax
// (See
// https://sourceware.org/gdb/current/onlinedocs/gdb/Packets.html#thread%2did%20syntax
// for reference).
std::string EncodeThreadId(mx_koid_t pid, mx_koid_t tid);

// Finds and returns the index of the first occurence of |val| within |packet|,
// such that it is not preceded by an escape character.
bool FindUnescapedChar(const char val,
                       const ftl::StringView& packet,
                       size_t* out_index);

// Verifies that the given command is formatted correctly and that the checksum
// is correct. Returns false verification fails. Otherwise returns true, and
// returns a pointer to the beginning of the packet data and the size of the
// packet data in the out parameters. A GDB Remote Protocol packet is defined
// as:
//
//   $<packet-data>#<2-digit checksum>
//
bool VerifyPacket(ftl::StringView packet, ftl::StringView* out_packet_data);

// Extracts the prefix and the parameters from |packet| and returns them in the
// |out_*| variables. The prefix and the parameters should be separated by a
// colon (':'). If |packet| does not contain a colon, or if there are no
// characters following a colon, the returned parameters will be an empty
// string. |packet| cannot be empty.
void ExtractParameters(const ftl::StringView& packet,
                       ftl::StringView* out_prefix,
                       ftl::StringView* out_params);

}  // namespace util
}  // namespace debugserver
