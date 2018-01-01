/****************************************************************
* String utilities
****************************************************************/
#include "string-util.hpp"

#include <algorithm>

using namespace std;

namespace util {

// Returns true if s contains what.
bool contains( string_view s, string_view what ) {
    return s.find( what ) != string_view::npos;
}

// Returns true if s starts with w.
bool starts_with( string_view s, string_view w ) {
    return mismatch( begin( s ), end( s ),
                     begin( w ), end( w) ).second == end( w );
}

// Returns true if s ends with what.
bool ends_with( string_view s, string_view w ) {
    return mismatch( rbegin( s ), rend( s ),
                     rbegin( w ), rend( w ) ).second == rend( w );
}

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
    return res;
}

// Split  a  string, strip all elements, and remove empty strings
// from result.
vector<string_view> split_strip( string_view sv, char c ) {

    auto res = split( sv, c );
    transform( begin( res ), end( res ), begin( res ), strip );
    auto new_end = remove_if(
            begin( res ), end( res ), std::empty<string_view> );
    res.erase( new_end, end( res ) );
    return res;
}

// Convert element type.
vector<string> to_strings( vector<string_view> const& svs ) {

    vector<string> res;
    for( auto sv : svs )
        res.emplace_back( sv );
    return res;
}

// Convert string to path
fs::path to_path( string_view sv ) {
    return fs::path( sv );
}

// Convert element type.
vector<fs::path> to_paths( vector<string> const& ss ) {

    vector<fs::path> res;
    for( auto s : ss )
        res.emplace_back( s );
    return res;
}

/****************************************************************
* To-String utilities
****************************************************************/
// NOTE: This puts quotes around the  string! The reason for this
// behavior  is that we want to try to perform the to_string oper-
// ation  (in general) such that it has some degree of reversibil-
// ity. For example, converting  the  integer  55  and the string
// "55" to strings should yield different  results so that we can
// distinguish the types from the  string representations (and or
// convert  back, at least approximately). So therefore, whenever
// the  to_string methods convert a already-string-like entity to
// a string, it will insert quotes in the string itself.
template<>
string to_string<string>( string const& s ) {
    return "\"" + s + "\"";
}

// NOTE:  This  puts  quotes around the string! Also, it is not a
// template  specialization  because  for  some reason gcc always
// wants to select the version for ints/floats below  instead  of
// this one when we give it string literals (i.e., type deduction
// is not doing what we want). But  having this one causes gcc to
// select it when we give it a string literal.
std::string to_string( char const* s ) {
    return "\"" + string( s ) + "\"";
}

// NOTE: This puts single quotes around the character!
template<>
string to_string<char>( char const& s ) {
    string res( 1, s );
    return "'" + res + "'";
}

// This one always throws an error because it would not be
// portable to allow this since on Windows  fs::path's  only  con-
// vert to wide strings. Though would be better  to  have  a  com-
// piler error rather than exception.
template<>
string to_string<fs::path>( fs::path const& ) {
   ERROR( "should not call this method as it is not portable" );
}

}
