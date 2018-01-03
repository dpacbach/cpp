/****************************************************************
* String utilities
****************************************************************/
#pragma once

#include "error.hpp"
#include "util.hpp"
#include "types.hpp"

#include <cctype>
#include <experimental/filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
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

    return std::equal( std::begin( s1 ), std::end( s1 ),
                       std::begin( s2 ), std::end( s2 ),
                       predicate );
}

// This  will  intersperse  `what` into the vector of strings and
// join the result. It  will  attempt  to compute require reserve
// space before hand to minimize memory allocations.
template<typename T>
std::string join( std::vector<T> const& v,
                  std::string_view      what ) {

    if( !v.size() ) return {};
    // First attempt to compute how much space we need, which  we
    // should be able to do exactly.
    size_t total = 0;
    for( auto const& e : v )
        total += e.size();
    total += what.size()*(v.size() - 1); // v.size() > 0 always
    // Now construct the result (reserve +1 for good measure).
    std::string res; res.reserve( total+1 );
    bool first = true;
    for( auto const& e : v ) {
        if( !first )
            res += what;
        res += e;
        first = false;
    }
    // Just to make sure  we  did  the  calculation right; if not,
    // then we might pay extra in memory allocations.
    ASSERT( res.size() == total, "res.size() == " << res.size() <<
                                 " and total == " << total );
    return res;
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
*
* util::to_string  family of overloaded functions are intended so
* that  a  user  can call them on any commonly-used type and they
* will return a sensibly  formatted result. Unlike std::to_string
* these overloads work on  various  containers  as  well, such as
* vectors and tuples. For simple  numeric  types  util::to_string
* delegates  to  std::to_string.  If  all else fails, the default
* overload  attempts  to use a string stream to do the conversion.
*
* See the special note below  on  the  std::string  overload.  In
* short, Whenever the to_string methods  convert a string (or any
* string-like entity) to a string, they will insert quotes in the
* string itself.
****************************************************************/
template<typename T>
std::string to_string( T const& );

// NOTE: This puts single quotes around the character!
template<>
std::string to_string<char>( char const& c );

// NOTE: These puts quotes around the string! The reason for this
// behavior  is that we want to try to perform the to_string oper-
// ation  (in general) such that it has some degree of reversibil-
// ity. For example, converting  the  integer  55  and the string
// "55" to strings should yield different  results so that we can
// distinguish the types from the string representations  (and/or
// convert  back, at least approximately). So therefore, whenever
// the  to_string methods convert a already-string-like entity to
// a string, it will insert quotes in the string itself.
template<>
std::string to_string<std::string>( std::string const& s );
template<>
std::string to_string<std::string_view>(
        std::string_view const& s );

// NOTE:  This  puts  quotes around the string! Also, it is not a
// template  specialization  because  for  some reason gcc always
// wants to select the version for ints/floats below  instead  of
// this one when we give it string literals (i.e., type deduction
// is not doing what we want). But  having this one causes gcc to
// select it when we give it a string literal.
std::string to_string( char const* s );

// This one simply throws an exception to prevent the  user  from
// calling it because it is  platform  dependent. Would be better
// though to have a compiler error.
template<>
std::string to_string<fs::path>( fs::path const& p );

// Simply delegate to the wrapped type.
template<typename T>
std::string to_string( std::reference_wrapper<T> const& rw ) {
    return util::to_string( rw.get() );
}

// Not  sure if this one is also needed, but doesn't seem to hurt.
template<typename T>
std::string to_string( std::reference_wrapper<T const> const& rw ) {
    return util::to_string( rw.get() );
}

template<typename T>
std::string to_string( std::optional<T> const& opt ) {
    return opt ? util::to_string( *opt )
               : std::string( "nullopt" );
}

// This function exists for the purpose of  having  the  compiler
// deduce the Indexes variadic integer arguments that we can then
// use to index the tuple; it probably is not useful to call this
// method  directly  (it is called by to_string). Was not able to
// find a more elegant way of unpacking an arbitrary tuple passed
// in as an argument apart from using  this  helper  function  in-
// volving the index_sequence.
template<typename Tuple, size_t... Indexes>
StrVec tuple_elems_to_string( Tuple const& tp,
                              std::index_sequence<Indexes...> ) {
    StrVec res; res.reserve( std::tuple_size_v<Tuple> );
    // Unary right fold of template parameter pack.
    ((res.push_back( util::to_string( std::get<Indexes>( tp ) ))), ...);
    return res;
}

// Will do JSON-like notation. E.g. (1,2,3)
template<typename... Args>
std::string to_string( std::tuple<Args...> const& tp ) {
    auto is = std::make_index_sequence<sizeof...(Args)>();
    auto v = tuple_elems_to_string( tp, is );
    return "(" + join( v, "," ) + ")";
}

// This function exists for the purpose of  having  the  compiler
// deduce the Indexes variadic integer arguments that we can then
// use  to  index  the variant; it probably is not useful to call
// this method directly (it is called by to_string).
template<typename Variant, size_t... Indexes>
std::string variant_elems_to_string(
        Variant const& v,
        std::index_sequence<Indexes...> ) {
    std::string res;
    // Unary right fold of template parameter pack.
    (( res += (Indexes == v.index())
            ? util::to_string( std::get<Indexes>( v ) ) : ""
    ), ...);
    return res;
}

template<typename... Args>
std::string to_string( std::variant<Args...> const& v ) {
    auto is = std::make_index_sequence<sizeof...(Args)>();
    return variant_elems_to_string( v, is );
}

template<>
inline std::string to_string<Error>( Error const& e )
    { return e.msg; }

// Will do JSON-like notation. E.g. (1,"hello")
template<typename U, typename V>
std::string to_string( std::pair<U, V> const& p ) {
    return "(" + util::to_string( p.first )  + ","
               + util::to_string( p.second ) + ")";
}

// Prints in JSON style notation. E.g. [1,2,3]
template<typename T>
std::string to_string( std::vector<T> const& v ) {

    std::vector<std::string> res( v.size() );
    // We  need  this lambda to help std::transform with overload
    // resolution of to_string.
    auto f = []( T const& e ){ return util::to_string( e ); };
    std::transform( std::begin( v   ), std::end( v ),
                    std::begin( res ), f );
    return "[" + join( res, "," ) + "]";
}

// Default  version uses std::to_string which is only defined for
// a few primitive types.
template<typename T>
std::string to_string( T const& arg ) {
    return std::to_string( arg );
}

template<typename T>
std::ostream& operator<<( std::ostream&         out,
                          std::vector<T> const& v ) {
    return (out << util::to_string( v ));
}

template<typename U, typename V>
std::ostream& operator<<( std::ostream&          out,
                          std::pair<U, V> const& p ) {
    return (out << util::to_string( p ));
}

}
