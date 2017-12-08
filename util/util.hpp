/****************************************************************
* Utilities
****************************************************************/
#pragma once

#include <algorithm>
#include <string>

// This is intended to lessen  typing  for the simplest of lambda
// functions, namely, those  which  have  no  captures,  take one
// const ref parameter, and consist of a single return  statement.
#define L( a )  [] ( auto const& _ ) { return a; }

// One  for  lambdas  that  capture  all (usually for simplicity).
#define LC( a ) [&]( auto const& _ ) { return a; }

namespace util {

template<typename T>
std::string to_string( T const& arg );

// Does the set contain the given key.
template<typename ContainerT, typename KeyT>
bool has_key( ContainerT const& s, KeyT const& k ) {
    return s.find( k ) != s.end();
}

// This  will  do  the remove/erase idiom automatically for conve-
// nience.
template<typename Container, typename Func>
void remove_if( Container& c, Func f ) {
    auto new_end = std::remove_if(
            std::begin( c ), std::end  ( c ), f );
    c.erase( new_end, end( c ) );
}

}
