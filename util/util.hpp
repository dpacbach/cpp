/****************************************************************
* Utilities
****************************************************************/
#pragma once

#include "types.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// This is intended to lessen  typing  for the simplest of lambda
// functions, namely, those  which  have  no  captures,  take one
// const ref parameter, and consist of a single return  statement.
#define L( a )  [] ( auto const& _ ) { return a; }

// One  for  lambdas  that  capture  all (usually for simplicity).
#define LC( a ) [&]( auto const& _ ) { return a; }

namespace util {

template<typename T>
std::string to_string( T const& arg );

// Loop through the elements in a vector and output them.
template<typename T>
void print_vec( std::vector<T> const& v,
                std::ostream&         out,
                bool                  indent = false,
                std::string_view      name = std::string_view() ) {
    if( !name.empty() )
        out << name << std::endl;
    std::string padding = indent ? "    " : "";
    for( auto const& e : v )
        out << padding << e << std::endl;
}

// Does the set contain the given key.
template<typename ContainerT, typename KeyT>
bool has_key( ContainerT const& s, KeyT const& k ) {
    return s.find( k ) != s.end();
}

// Get  a reference to a value in a map. Since the key may not ex-
// ist, we return an optional. But  since  we want a reference to
// the  object,  we  return  an  optional  of a reference wrapper,
// since  containers can't hold references. I think the reference
// wrapper returned here should only allow const references to be
// extracted.
template<
    typename KeyT,
    typename ValT,
    template<typename KeyT_, typename ValT_>
    typename MapT
>
OptRef<ValT const> get_key( MapT<KeyT,ValT> const& m,
                            KeyT            const& k ) {
    auto found = m.find( k );
    if( found == m.end() )
        return std::nullopt;
    return found->second;
}

// This  will  do  the remove/erase idiom automatically for conve-
// nience.
template<typename Container, typename Func>
void remove_if( Container& c, Func f ) {
    auto new_end = std::remove_if(
            std::begin( c ), std::end  ( c ), f );
    c.erase( new_end, end( c ) );
}

// Will do an in-place sort and unique.
template<typename T>
void uniq_sort( std::vector<T>& v ) {
    std::sort( begin( v ), end( v ) );
    auto i = std::unique( begin( v ), end( v ) );
    v.erase( i, end( v ) );
}

}

// For a vector whose element type can be  streamed,  output  the
// vector in a JSON-like syntax.
template<typename T>
std::ostream& operator<<( std::ostream&         out,
                          std::vector<T> const& v ) {
    out << "[";
    bool first = true;
    for( auto const& i : v ) {
        if( !first )
            out << ",";
        out << i;
        first = false;
    }
    out << "]";
    return out;
}
