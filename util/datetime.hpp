/****************************************************************
* Date/Time Utilities
****************************************************************/
#pragma once

#include "types.hpp"

#include <chrono>
#include <string>

namespace util {

// We  use  seconds  as the most general way to represent the dif-
// ference  between  two time zones at a given point in time, and
// use this typedef for readability.  The  sign  of a TZOffset is
// such  that  it  should  be  added  to a UTC time to obtain the
// target  time  zone.  If  it is zero, that represents UTC. Note
// that values of this type should not be cached because they can
// change from day to day as e.g. daylight savings time starts or
// stops.
using TZOffset = std::chrono::seconds;

// Return  the  offset in seconds from the local time zone to UTC.
TZOffset tz_local();
// This one is to enable readability.
inline TZOffset tz_utc() { return TZOffset( 0 ); }

// Returns  a string representation of the offset between UTC and
// the  time zone descibed by the given TZOffset. The string will
// be in the format (+/-)hhmm, e.g. "-0500" for New York, "+0000"
// for UTC. NOTE: the reason  that  we  are implementing this our-
// selves is because  it  seems  that  the  strftime (and related
// methods) are not able to correctly emit this string on Windows
// under MinGW, which they  do  on  Linux  with  the %z formatter.
std::string tz_hhmm( TZOffset off = tz_local() );

/****************************************************************
* Type-safe time point types
****************************************************************/
// This is just a trivial wrapper around a time_point  that  tags
// it has having the interpretation of referring to a local time;
// this means that it cannot be interpreted as an absolute  point
// in  time  without the additional piece of information of which
// time zone it refers to (which it does not contain).
template<typename Clock> struct local_time_point {

public:
    // Time Point Type
    using point_t = typename Clock::time_point;

private:
    point_t tp;

public:
    // This constructor says "interpret  the  given time point pa-
    // rameter as a local time." We don't require that  this  con-
    // structor be explicit because it  is  fine  for a caller to
    // pass a point_t to  a  function  expecting  a local time be-
    // cause in this library  chrono  time points are interpreted
    // as local times.
    local_time_point( point_t const& in_tp ) : tp( in_tp ) {}

    // Get underlying chrono time  point  which,  in this library,
    // we also interpret as a local time.
    point_t const& get() const { return tp; }

    // Since in this library chrono time points  are  interpreted
    // as local times, it is  fine  to allow automatic conversion
    // of this to a chrono time point.
    operator point_t const&() const { return tp; }
};

using LocalTimePoint = local_time_point<std::chrono::system_clock>;

// This is just a trivial wrapper around a time_point  that  tags
// it has having an interpretation  of  referring to UTC time. Of
// importance is that it  does  not  allow construction or conver-
// sion to or from the underlying chrono time point.
template<typename Clock> struct zoned_time_point {

    // Local time Point Type
    using local_t = local_time_point<Clock>;

    local_t tp;

    // Delete any constructors that don't force user to specify a
    // a time zone with which to interpret the time point. We  ex-
    // plicitely delete these for documentation purposes.
    zoned_time_point() = delete;
    zoned_time_point( local_t const& p ) = delete;

public:
    // Construct a zoned (absolute) time point given a local time
    // time point and its offset from UTC.
    zoned_time_point( local_t const& in_tp, TZOffset off )
        : tp( in_tp.get() - off ) {}

    LocalTimePoint to_local( TZOffset off ) const
        { return local_t( tp.get() + off ); }
};

using ZonedTimePoint = zoned_time_point<std::chrono::system_clock>;

/****************************************************************
* Time formatting
****************************************************************/
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
// particular  time zone and so will not perform any offset compu-
// tations  or  attach  any time zone labels. In other words, one
// can  imagine  that the time zone passed in will be interpreted
// as an offset from the Unix Time Epoch, which is then formatted
// as a string but without its UTC time zone indicator.
//
// Note  that  strings  of  this form are useful because two such
// strings can be compared  lexicographically to compare ordering,
// though  of  course  this only works when the times are with re-
// spect to the same time one.
//
// This function takes a particular kind of  time  point  because
// apparently the system clock time point is the  only  one  that
// can  be  converted to time_t, which we need to do to output it.
std::string fmt_time( LocalTimePoint const& p );

// Formats the string as for the LocalTimePoint but also attaches
// a  timezone  offset  since  that information is available. Fur-
// thermore  the  timezone  with  respect  to which the result is
// written  can  be  specified in the second argument. The output
// will always be precisely 34 characters  long and will have the
// format:
//
//     2018-01-15 15:52:48.421397398-0500
// or
//     2018-01-15 20:52:48.421397398+0000
//
// NOTE: these strings cannot be  compared  lexicographically  un-
// less the timezones are the same.
std::string fmt_time( ZonedTimePoint const& p,
                      TZOffset off = tz_local() );

} // namespace util

// For convenience, dump these  two  into  the  global  namespace.
using util::LocalTimePoint;
using util::ZonedTimePoint;
