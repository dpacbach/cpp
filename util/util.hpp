/****************************************************************
* Utilities
****************************************************************/
#pragma once

#include <string>

namespace util {

template<typename T>
std::string to_string( T const& arg );

// Does the set contain the given key.
template<typename ContainerT, typename KeyT>
bool has_key( ContainerT const& s, KeyT const& k ) {
    return s.find( k ) != s.end();
}

}
