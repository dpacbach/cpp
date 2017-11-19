/****************************************************************
* String utilities
****************************************************************/
#include "string-util.hpp"

using namespace std;

namespace util {

// Strip all blank space off of a string view and return
// a new one.
string_view strip( string_view sv ) {

    auto start = sv.find_first_not_of( " \t\n\r" );
    sv.remove_prefix( min( start, sv.size() ) );
    // Must do this here because sv is being mutated.
    auto last  = sv.find_last_not_of( " \t\n\r" )+1;
    sv.remove_suffix( sv.size() - min( last, sv.size() ) );
    return sv;

}

}
