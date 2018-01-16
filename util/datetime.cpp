/****************************************************************
* Utilities
****************************************************************/
#include "datetime.hpp"
#include "macros.hpp"

#include <ctime>
#include <iomanip>

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

    // These functions do  the  same  thing,  apart from timezone.
    auto* tm = utc ? std::gmtime( &t ) : std::localtime( &t );

    ostringstream ss;

    // The  %z outputs nothing if there is no time zone in the tm,
    // but  that  should  not  happen here because we're starting
    // from a time point. Though if for some reason it does, then
    // it will cause the resulting string  to be shorter and will
    // then trigger the  assertion  on  the  string  length below.
    ss << put_time( tm, "%Y-%m-%d %T" ) << "." << ns.count()
       << put_time( tm, "%z" );

    string res = ss.str();

    ASSERT( res.size() == 34, "formatted string " << res <<
                              "has unexpected length." );
    return res;
}

} // util
