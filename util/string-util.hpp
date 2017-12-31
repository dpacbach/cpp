/****************************************************************
* String utilities
****************************************************************/
#pragma once

#include "util.hpp"

#include <cctype>
#include <experimental/filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace util {

// Returns true if s contains what.
bool contains( std::string_view s, std::string_view what );

// Returns true if s starts with what.
bool starts_with( std::string_view s, std::string_view what );

// Returns true if s ends with what.
bool ends_with( std::string_view s, std::string_view what );

// Case-insensitive comparison. This is intended to work for both
// char strings and wchar strings.
template<typename StringT>
bool iequals( StringT const& s1, StringT const& s2 ) {
    // This check is for efficiency.
    if( s1.size() != s2.size() )
        return false;

    auto predicate = []( auto l, auto r ) {
        if constexpr( sizeof( StringT ) == 1 )
            return (std::tolower( l ) == std::tolower( r ));
        else {
            int l_i( l ), r_i( r );
            if( l_i > 127 || r_i > 127 )
                // not sure how to make higher-order chars  lower-
                // case, so just compare them.
                return (l_i == r_i);
            else
                return (tolower( l_i ) == tolower( r_i ));
        }
    };

    return std::equal( begin( s1 ), end( s1 ),
                       begin( s2 ), end( s2 ),
                       predicate );
}

// Strip all blank space off of a string view and return
// a new one.
std::string_view strip( std::string_view sv );

// Split a string on a character.
std::vector<std::string_view>
split( std::string_view sv, char c );

// Split a string, strip all elements, and remove empty strings
// from result.
std::vector<std::string_view>
split_strip( std::string_view sv, char c );

// Convert element type.
std::vector<std::string>
to_strings( std::vector<std::string_view> const& svs );

// Convert string to path
fs::path to_path( std::string_view sv );

// Convert element type.
std::vector<fs::path>
to_paths( std::vector<std::string> const& ss );

/****************************************************************
* To-String utilities
****************************************************************/
// Default version calls std::to_string which covers all of the
// basic primitive types.
template<typename T>
std::string to_string( T const& arg ) {
    return std::to_string( arg );
}

// NOTE: This puts quotes around the string!
template<>
std::string to_string<std::string>( std::string const& s );

// NOTE: This puts quotes around the string!
template<>
std::string to_string<char const*>( char const* const& s );

// Trivial; extract string from path.
template<>
std::string to_string<fs::path>( fs::path const& p );

template<typename T>
std::string to_string( std::optional<T> const& opt ) {
    return opt ? util::to_string( *opt )
               : std::string( "nullopt" );
}

}
