/****************************************************************
* Utilities
****************************************************************/
#pragma once

#include "macros.hpp"
#include "types.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// This is intended to lessen  typing  for the simplest of lambda
// functions, namely, those  which  have  no  captures,  take one
// const ref parameter, and  consist  of  either  a single return
// statement or just a single expression.
#define L( a )  [] ( auto const& _ ) { return a; }
#define L_( a ) [] ( auto const& _ ) { a; }

// One  for  lambdas  that  capture  all (usually for simplicity).
#define LC( a )  [&]( auto const& _ ) { return a; }
#define LC_( a ) [&]( auto const& _ ) { a; }

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
        out << name << "\n";
    std::string padding = indent ? "    " : "";
    for( auto const& e : v )
        out << padding << e << "\n";
}

// Does the set contain the given key.
template<typename ContainerT, typename KeyT>
bool has_key( ContainerT const& s, KeyT const& k ) {
    return s.find( k ) != s.end();
}

// The idea of this function is that it will test the given key's
// membership (as a key) in the map and, if it is found, it  will
// return  a  reference  (in  an optional) to that key inside the
// container. Again, it guarantees to not only return a reference
// to  the  key, but it will be a reference to the one in the con-
// tainer, which will then only live as  long  as  the  map  does.
//
// The reason for the return semantics is that 1) if  the  caller
// eventually  wants  to  copy the returned key, they can do that
// regardless  of which reference we return, but 2) if the caller
// wants  to  hang on to a reference to the key (when found) then
// it is more likely they will want a reference to the one in the
// map,  for  reasons of managing lifetime of the object (key) re-
// ferred to.
template<typename ContainerT, typename KeyT>
OptCRef<KeyT> get_key_safe( ContainerT const& m,
                            KeyT       const& k ) {
    auto found = m.find( k );
    if( found == m.end() )
        return std::nullopt;
    // Must return the one in the container, not the  one  passed
    // in as an argument, that's the idea here.
    return found->first;
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
OptRef<ValT const> get_val_safe( MapT<KeyT,ValT> const& m,
                                 KeyT            const& k ) {
    auto found = m.find( k );
    if( found == m.end() )
        return std::nullopt;
    return found->second;
}

// Get value for key; if key does not exist it will throw. If  it
// does exist it will return  a  reference  to  the object in the
// container. ***NOTE that, for error reporting, we are  assuming
// that the key type can be output via ostream, which seems  like
// a reasonable assumption for most things that are normally used
// as keys.
template<
    typename KeyT,
    typename ValT,
    template<typename KeyT_, typename ValT_>
    typename MapT
>
ValT const& get_val( MapT<KeyT,ValT> const& m,
                     KeyT            const& k ) {
    auto found = m.find( k );
    ASSERT( found != m.end(), k << " not found in map" );
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

}

// Here  we  open up the std namespace to add a hash function spe-
// cialization for a reference_wrapper. This  is  straightforward
// because we can just delegate to the specialization of the type
// inside the wrapper, if there is one. If there is not one, then
// this  specialization  will not be used (which is what we want).
namespace std {

    // Specializing hash<>
    template<>
    // Adding template for contents of reference_wrapper
    template<typename T>
    struct hash<reference_wrapper<T>> {
        auto operator()(
                reference_wrapper<T> const& p ) const noexcept {
            return hash<remove_const_t<T>>{}( p.get() );
        }
    };

} // namespace std
