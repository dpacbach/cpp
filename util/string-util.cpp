/****************************************************************
* String utilities
****************************************************************/
#include "string-util.hpp"

#include <algorithm>

using namespace std;

namespace util {

// Strip all blank space off of  a  string  view and return a new
// one.
string_view strip( string_view sv ) {

    auto start = sv.find_first_not_of( " \t\n\r" );
    sv.remove_prefix( min( start, sv.size() ) );
    // Must do this here because sv is being mutated.
    auto last  = sv.find_last_not_of( " \t\n\r" )+1;
    sv.remove_suffix( sv.size() - min( last, sv.size() ) );
    return sv;

}

// Split a string on a character.
vector<string_view> split( string_view sv, char c ) {

    vector<string_view> res;
    while( true ) {
        auto next = sv.find_first_of( c );
        if( next == string_view::npos ) break;
        res.push_back( sv.substr( 0, next ) );
        // Remove what we just added, plus c
        sv.remove_prefix( next+1 );
    }
    res.push_back( sv );
    return move( res );
}

// Split  a  string, strip all elements, and remove empty strings
// from result.
vector<string_view> split_strip( string_view sv, char c ) {

    auto res = split( sv, c );
    transform( begin( res ), end( res ), begin( res ), strip );
    auto new_end = remove_if(
            begin( res ), end( res ), std::empty<string_view> );
    res.erase( new_end, end( res ) );
    return move( res );
}

// Convert element type.
vector<string> to_strings( vector<string_view> const& svs ) {

    vector<string> res;
    for( auto sv : svs )
        res.emplace_back( sv );
    return move( res );
}

}
