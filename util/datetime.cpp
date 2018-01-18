/****************************************************************
* Utilities
****************************************************************/
#include "datetime.hpp"
#include "macros.hpp"

#include <array>
#include <cmath>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include "Windows.h"
#endif

using namespace std;
using namespace chrono;

namespace util {

// Return  the  offset in seconds from the local time zone to UTC.
TZOffset tz_local() {
#ifndef _WIN32
    // The value of this variable matters not  because  we're  in-
    // terested in the current time (we will only be sampling the
    // tm_gmtoff field from the result  which  is a function only
    // of timezone offset)  but  instead  because the localtime_r
    // function will decide based on the current time which phase
    // of daylight savings time  we  are  in  which will be added
    // into tm_gmtoff.
    time_t time = ::time( NULL );
    // This will be populated by localtime_r.
    tm local;
    // localtime_r is a threadsafe version  of localtime since it
    // does  not  mutate  any  global  data, though it may not be
    // fully portable.
    localtime_r( &time, &local );

    return seconds( local.tm_gmtoff );
#else
    TIME_ZONE_INFORMATION lpTimeZoneInformation;
    auto ds = GetTimeZoneInformation(
                  &lpTimeZoneInformation );
    // Bias is difference in minutes between UTC and local.
    auto offset_min = lpTimeZoneInformation.Bias;
    if( ds == TIME_ZONE_ID_DAYLIGHT )
        offset_min += lpTimeZoneInformation.DaylightBias;
    return seconds( -60*offset_min );
#endif
}

// Returns  a string representation of the offset between UTC and
// local  time in the format (+/-)hhmm, e.g. "-0500" for New York,
// "+0000"  for  UTC.  NOTE:  the reason that we are implementing
// this ourselves is because it seems that the strftime  (and  re-
// lated  methods)  are not able to correctly emit this string on
// Windows under MinGW, which they  do  on  Linux with the %z for-
// matter.
string tz_hhmm( TZOffset off ) {

    auto secs = off;
    ostringstream ss; ss.fill( '0' );
    auto sign = (secs < seconds( 0 )) ? '-' : '+';
    secs      = (secs < seconds( 0 )) ? -secs : secs;
    // Since secs is supposed  to  represent  the total number of
    // seconds in a time zone offset, it must be less than  24hrs
    // as a sanity check.
    ASSERT_( secs < hours( 24 ) );
    // This will round down to hours.
    auto hrs  = duration_cast<hours>( secs );
    // Total number of residual minutes after all multiples of 60
    // minutes (i.e., 1hr) are subtracted. Note that secs  is  al-
    // ways >=0 at this point.
    auto mins = duration_cast<minutes>( secs - hrs );
    ss << sign << setw( 2 ) << hrs.count()
               << setw( 2 ) << mins.count();
    auto res = ss.str();
    ASSERT( res.size() == 5, "timezone offset " << res << " has "
                             "unexpected length (not 5)." );
    return res;
}

// Formats a time_t with the following format, which is the  stan-
// dard format used by this library:
//
//   2018-01-15 20:52:48
//
// Neither  assuming nor attaching information about time zone to
// the time. Strings of this form can  be  compared  lexicographi-
// cally for he purposes of comparing by time ordering.
string fmt_time( time_t t ) {

    // Populate a tm struct with the results of converting number
    // of  seconds  since  the  epoch  time to calendar date. The
    // epoch time traditionally refers to UTC time zone, but this
    // is not relevant here.
    tm cal_time; gmtime_r( &t, &cal_time );

    // Place to put the  result.  Size  of  result including null
    // zero; compute it from template  to  avoid magic numbers in
    // code.
    array<char, sizeof( "0000-00-00 00:00:00" )> cs;

    char* const start = begin( cs );

    char const* fmt = "%Y-%m-%d %H:%M:%S";

    // strftime() returns # of chars written not included \0.
    ASSERT( cs.size()-1 ==
            strftime( begin( cs ), cs.size(), fmt, &cal_time ),
           "failed to write " << cs.size() << "chars in result "
           "of time_t formatting" );

    return string( start, cs.size()-1 );
}

// See header file for more  in-depth explanation. In short, this
// function formats  a  time_point  with  the  following  format:
//
//     2018-01-15 20:52:48.421397398
//
// i.e., with no assumptions or  interpretations  about  timezone.
string fmt_time( LocalTimePoint const& p ) {

    using namespace literals::chrono_literals;

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
    // result happen to be less than one second, which by the way
    // happens  to be the case here, but not in general, since we
    // can  have  an  arbitrary  quantity  of  nanoseconds  corre-
    // sponding to a duration longer than a second).
    chrono::nanoseconds ns =
        p.get() - std::chrono::system_clock::from_time_t( t );

    // The following is not guaranteed by the types, but we  know
    // it must be true in this function, so do  it  as  a  sanity
    // check.
    ASSERT_( ns < 1s );

    ostringstream ss; ss.fill( '0' );

    // Start with the date/time  base  which  we can extract from
    // the time_t, then add in nanoseconds.
    ss << fmt_time( t ) << "." << setw( 9 ) << ns.count();

    string res = ss.str();

    ASSERT( res.size() == 29, "formatted string " << res <<
                              " has unexpected length." );
    return res;
}

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
string fmt_time( ZonedTimePoint const& p, TZOffset off ) {
    return fmt_time( p.to_local( off ) ) + tz_hhmm( off );
}

} // util
