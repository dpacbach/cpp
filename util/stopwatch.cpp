/****************************************************************
* StopWatch
****************************************************************/
#include "macros.hpp"
#include "stopwatch.hpp"
#include "util.hpp"

#include <sstream>

using namespace std;

namespace util {

// Start the clock for a given event  name. If an event with this
// name  already  exists  then it will be overwritten and any end
// times for it will be deleted.
void StopWatch::start( string_view name ) {
    string n( name );
    start_times[n] = clock_type::now();
    if( has_key( end_times, n ) )
        end_times.erase( n );
}

// Register an end time for an event. Will throw if there was  no
// start time for the event.
void StopWatch::stop( string_view name ) {
    string n( name );
    ASSERT_( has_key( start_times, n ) );
    end_times[n] = clock_type::now();
}

// Get results for an even in the given units. If either a  start
// or  end  time for the event has not been registered then these
// will throw.
int64_t StopWatch::milliseconds( string_view name ) const {
    ASSERT_( event_complete( name ) );
    string n( name );
    return chrono::duration_cast<chrono::milliseconds>(
        end_times.at( n ) - start_times.at( n ) ).count();
}

int64_t StopWatch::seconds( string_view name ) const {
    ASSERT_( event_complete( name ) );
    string n( name );
    return chrono::duration_cast<chrono::seconds>(
        end_times.at( n ) - start_times.at( n ) ).count();
}

int64_t StopWatch::minutes( string_view name ) const {
    ASSERT_( event_complete( name ) );
    string n( name );
    return chrono::duration_cast<chrono::minutes>(
        end_times.at( n ) - start_times.at( n ) ).count();
}

// Gets the results for an event  and  then formats them in a way
// that is most readable given the duration.
string StopWatch::human( string_view name ) const {
    ASSERT_( event_complete( name ) );
    ostringstream out;
    // Each  of  these represent the same time, just in different
    // units.
    auto m  = minutes( name );
    auto s  = seconds( name );
    auto ms = milliseconds( name );
    if( m > 0 )
        out << m << "m" << s % 60 << "s";
    else if( s > 0 ) {
        out << s;
        if( s < 10 )
            out << "." << ms % 1000;
        out << "s";
    }
    else
        out << ms << "ms";
    return out.str();
}

// Get a list of all results in human readable form.
vector<StopWatch::result_pair> StopWatch::results() const {
    vector<result_pair> res;
    for( auto const& p : start_times ) {
        ASSERT( event_complete( p.first.c_str() ),
            "event " << p.first << " is not complete." );
        res.emplace_back( make_pair(
            p.first, human( p.first ) ) );
    }
    return res;
}

// Will simply check if an event is present in both the start and
// end time sets, i.e., it is ready for computing results.
bool StopWatch::event_complete( string_view name ) const {
    string n( name );
    return has_key( start_times, n ) &&
           has_key( end_times,   n );
}

} // namespace util
