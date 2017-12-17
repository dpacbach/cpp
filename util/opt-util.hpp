/****************************************************************
* std::optional utilities
****************************************************************/
#pragma once

#include "util.hpp"

#include <iostream>
#include <optional>

namespace util {

template<typename T>
std::string to_string( std::optional<T> const& opt ) {
    return opt ? std::string( *opt ) : std::string( "nullopt" );
}

} // namespace util

// In global namespace.
template<typename T>
std::ostream& operator<<( std::ostream&           out,
                          std::optional<T> const& opt ) {
    // In the implementation of  this  function we don't delegate
    // to  the  to_string method of optional that we have because
    // we  don't  want to assume that the type T can be converted
    // to a string necessarily  (all  we  need  is  for  it to be
    // streamable).
    if( opt )
        return (out << *opt);
    return (out << "nullopt");
}
