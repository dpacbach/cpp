/****************************************************************
* String utilities
****************************************************************/
#pragma once

#include "util.hpp"
#include "types.hpp"

#include <cctype>
#include <experimental/filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
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
template<typename T>
std::string to_string( T const& );

// NOTE: This puts single quotes around the character!
template<>
std::string to_string<char>( char const& c );

// NOTE: This puts quotes around the string!
template<>
std::string to_string<std::string>( std::string const& s );

// NOTE:  This  puts  quotes around the string! Also, it is not a
// template  specialization  because  for  some reason gcc always
// wants to select the version for ints/floats below  instead  of
// this one when we give it string literals (i.e., type deduction
// is not doing what we want). But  having this one causes gcc to
// select it when we give it a string literal.
std::string to_string( char const* s );

// Trivial; extract string from path.
template<>
std::string to_string<fs::path>( fs::path const& p );

// Prints in JSON style notation.
template<typename T>
std::string to_string( std::vector<T> const& v ) {
    std::string res = "[";
    bool first = true;
    for( auto const& i : v ) {
        if( !first )
            res += ",";
        res += util::to_string( i );
        first = false;
    }
    res += "]";
    return res;
}

template<typename T>
std::string to_string( std::reference_wrapper<T> const& rw ) {
    return util::to_string( rw.get() );
}

template<typename T>
std::string to_string( std::reference_wrapper<T const> const& rw ) {
    return util::to_string( rw.get() );
}

template<typename T>
std::string to_string( std::optional<T> const& opt ) {
    return opt ? util::to_string( *opt )
               : std::string( "nullopt" );
}

template<typename Tuple, size_t... Indexes>
StrVec tuple_elems_to_string( Tuple const& tp,
                              std::index_sequence<Indexes...> ) {
    StrVec res; res.reserve( std::tuple_size_v<Tuple> );
    // Unary right fold of template parameter pack.
    ((res.push_back( util::to_string( std::get<Indexes>( tp ) ))), ...);
    return res;
}

template<typename... Args>
std::string to_string( std::tuple<Args...> const& tp ) {
    auto is = std::make_index_sequence<sizeof...(Args)>();
    auto v = tuple_elems_to_string( tp, is );
    std::string res = "(";
    bool first = true;
    for( auto const& i : v ) {
        if( !first )
            res += ",";
        res += i;
        first = false;
    }
    res += ")";
    return res;
}

// Default version using either std::to_string or string  streams
// if that can't be used.
template<typename T>
std::string to_string( T const& arg ) {
    // This check is done at compile time.
    if constexpr( std::is_integral_v<T> ||
                  std::is_floating_point_v<T> ) {
        // Specialization  for  primitive types for which we will
        // just call std::to_string assuming that will be
        // fastest.
        return std::to_string( arg );
    } else {
        std::ostringstream oss; oss << arg;
        return oss.str();
    }
}

template<typename T>
std::ostream& operator<<( std::ostream&         out,
                          std::vector<T> const& v ) {
    return (out << util::to_string( v ));
}

}
