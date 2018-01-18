/****************************************************************
* Date/Time Utilities
****************************************************************/
#pragma once

#include "types.hpp"

#include <chrono>
#include <string>

namespace util {

// Return  the  offset in seconds from the local time zone to UTC.
std::chrono::seconds tz_local();

// Returns  a string representation of the offset between UTC and
// local  time in the format (+/-)hhmm, e.g. "-0500" for New York,
// "+0000"  for  UTC.  NOTE:  the reason that we are implementing
// this ourselves is because it seems that the strftime  (and  re-
// lated  methods)  are not able to correctly emit this string on
// Windows under MinGW, which they  do  on  Linux with the %z for-
// matter.
std::string tz_hhmm();

// Formats a time_t with the following format, which is the  stan-
// dard format used by this library:
//
//   2018-01-15 20:52:48
//
// Neither  assuming nor attaching information about time zone to
// the time. Strings of this form can  be  compared  lexicographi-
// cally for he purposes of comparing by time ordering.
std::string fmt_time( time_t t );

// Formats a time_point with the following format:
//
//   2018-01-15 20:52:48.421397398
//
// The output will always be precisely 29 characters long; if the
// time point given to the function does  not  have  nano  second
// precision then latter digits may just be  padded  with  zeroes.
// This  function does not interpret the time as referring to any
// particular  time  zone and will not make any assumptions about
// time  zone  in  general.  The time is taken "as is"; what this
// means is that  the  `duration`  object  held  inside  the time
// point, which by de-factor  standard  (though not official stan-
// dard)  refers  to  the  amount  of time elapsed since the Unix
// Epoch Time, will be interpreted as the amount of time  elapsed
// since a time point obtained by taking the Unix Epoch time  and
// replacing its UTC qualifier with  whatever the local time zone
// is. Though his is  done  implicitely;  this  function does not
// know what the local time zone is.
//
// Note  that  strings  of  this form are useful because two such
// strings can be compared  lexicographically to compare ordering,
// though  of  course  this only works when the times are with re-
// spect to the same time one.
//
// This function takes a particular kind of  time  point  because
// apparently the system clock time point is the  only  one  that
// can  be  converted to time_t, which we need to do to output it.
std::string fmt_time( SysTimePoint p );

} // namespace util
