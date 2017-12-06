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

template<typename T>
std::ostream& operator<<( std::ostream&           out,
                          std::optional<T> const& opt ) {
    return (out << to_string( opt ));
}

}
