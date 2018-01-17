/****************************************************************
* Utilities
****************************************************************/
#include "datetime.hpp"
#include "macros.hpp"

#include <cmath>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include "Windows.h"
#endif

using namespace std;

namespace util {

// This is a mutex that this  module uses whenever calling a func-
// tion that returns a pointer to an internal  std::tm  structure,
// since  apparently  those  functions  are not thread safe since
// they may mutate a global structure  and return a pointer to it.
mutex tm_mtx;

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
//
// Unfortunately it seems that timezones and time zone formatting
// using the standard POSIX C functions don't  really  work  prop-
// erly on Windows (at least  under  mingw)  at  the time of this
// writing. In particular, the localtime function seems unable to
// deduce timezone and to create a correct tm struct representing
// a local time. Also the formatter does not  properly  emit  the
// time zone offset (%z). Hence, we have some preprocessor  state-
// ments  below that will perform time zone calculations based on
// the results from Windows API functions in  order  to  get  the
// final result that we want, namely, that we can take the system
// time point, convert it to a time_t, then format that time_t in
// either UTC or local time.
string fmt_time_point( SystemTimePoint p, bool utc ) {

    using namespace literals::chrono_literals;

    // We need this because  apparently  the gmtime and localtime
    // functions below are not thread safe.
    lock_guard<mutex> lock( tm_mtx );

    // Get a time_t from the time_point. Note  that  time_t  will
    // lose all sub-second resolution.
    auto t = std::chrono::system_clock::to_time_t( p );

    // Now convert the time_t back  to  a time_point and subtract
    // it from the original time point,  effectively  leaving  us
    // with  number  of  nanoseconds which were lost in the above
    // conversion to time_t. Then we  auto  convert  it to a nano
    // seconds type by assigning it  to  a  variable of that type
    // explicitely. This conversion can be done without any casts
    // because it is not a lossless  conversion (this is the case
    // regardless of whether the total number of nanoseconds that
    // result happen to be  less  than  one  second, which is the
    // case here but not in general).
    chrono::nanoseconds ns =
        p - std::chrono::system_clock::from_time_t( t );

    // The following is not guaranteed by the types, but we  know
    // it must be true in this function, so do  it  as  a  sanity
    // check.
    ASSERT_( ns < 1s );

#ifdef _WIN32
    // In this block we are essentially  going to ask the Windows
    // API to give us  the  number  of minutes difference between
    // UTC  and  and the local time zone (accounting for daylight
    // savings time) and then  manually  add  that  to the time_t
    // computed  above  in  order to compensate for the fact that
    // the localtime function doesn't seem  to work (or is unreli-
    // able). Also using this information we compute the timezone
    // offset in %z format which the Windows strftime also  seems
    // incapable of doing.
    long abs_offset_hr, mod_offset_min; char sign;
    if( !utc ) {
        TIME_ZONE_INFORMATION lpTimeZoneInformation;
        auto ds = GetTimeZoneInformation(
                      &lpTimeZoneInformation );
        // Get offset in minutes between  UTC time and local time.
        auto offset_min = lpTimeZoneInformation.Bias;
        if( ds == TIME_ZONE_ID_DAYLIGHT )
            offset_min += lpTimeZoneInformation.DaylightBias;
        sign = (offset_min > 0) ? '-' : '+';

        t -= offset_min*60; // in seconds

        auto abs_offset_min = std::abs( offset_min );
        abs_offset_hr  = abs_offset_min / 60;
        mod_offset_min = abs_offset_min % 60;
    } else {
        abs_offset_hr = 0, mod_offset_min = 0;
        sign = '+';
    }

    // On Windows just compute the time as if it is  a  UTC  time
    // because, if the caller has  requested  local time, we have
    // already adjusted the time t to account for that.
    auto* tm = gmtime( &t );
#else
    // Linux.  Simple,  just  call  the C API to populate the cal-
    // endar object from a  Unix  time  for  each respective time
    // zone.
    auto* tm = utc ? gmtime( &t ) : localtime( &t );
#endif

    ostringstream ss; ss.fill( '0' );

    // The  %z outputs nothing if there is no time zone in the tm,
    // but  that  should  not  happen here because we're starting
    // from a time point. Though if for some reason it does, then
    // it will cause the resulting string  to be shorter and will
    // then trigger the  assertion  on  the  string  length below.
    ss << put_time( tm, "%Y-%m-%d %H:%M:%S" ) << "." << setw( 9 )
       << ns.count();

#ifdef _WIN32
    // On Windows unfortunately we need to manually construct the
    // [+-]hhmm  timezone  offset  since the C API isn't reliable.
    ss << sign << setw( 2 ) << abs_offset_hr << setw( 2 )
       << mod_offset_min;
#else
    ss << put_time( tm, "%z" );
#endif

    string res = ss.str();

    ASSERT( res.size() == 34, "formatted string " << res <<
                              " has unexpected length." );
    return res;
}

} // util
