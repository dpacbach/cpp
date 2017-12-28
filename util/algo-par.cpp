/****************************************************************
* Parallel Algorithms
****************************************************************/
#include "algo-par.hpp"

#include <thread>

using namespace std;

namespace util::par {

// Will  return  the max number of simultaneous threads supported
// on this system. Result will always be >= 1.
int max_threads() {
    if( auto ts = thread::hardware_concurrency(); ts >= 1 )
        return int( ts );
    return 1;
}

// Will take a vector of functions and will run  each  one  in  a
// separate  thread. The functions are expected to take no parame-
// ters  and  to  return  no values. This is a somewhat low-level
// function that should probably not be called  except  by  other
// functions in this module.
void in_parallel( vector<function<void()>> const& v ) {

    vector<thread> ts; ts.reserve( v.size() );

    for( auto const& e : v ) ts.emplace_back( e );

    for( auto& t : ts ) t.join();
}

} // namespace util::par
