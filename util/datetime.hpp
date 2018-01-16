/****************************************************************
* Date/Time Utilities
****************************************************************/
#pragma once

#include "types.hpp"

#include <chrono>
#include <mutex>
#include <string>

namespace util {

// This is a mutex that this  module uses whenever calling a func-
// tion that returns a pointer to an internal  std::tm  structure,
// since  apparently  those  functions  are not thread safe since
// they may mutate a global structure  and return a pointer to it.
extern std::mutex tm_mtx;

// Formats a time_point with the following format:
//
//   2018-01-15 20:52:48.421397398-0500
//
// The output will always be precisely 34  characters  long.  The
// timezone offset at the end will default to local,  unless  the
// utc  flag  is true in which case it will be +0000 and the rest
// of the time adjusted accordingly.
//
// Note  that  strings  of  this form are useful because two such
// strings can be compared  lexicographically to compare ordering,
// though this only works when  the  time zone offsets (last four
// digits) are the same.
//
// This function takes a particular kind of  time  point  because
// apparently the system clock time point is the  only  one  that
// can  be  converted to time_t, which we need to do to output it.
std::string fmt_time_point( SystemTimePoint p, bool utc = false );

} // namespace util
