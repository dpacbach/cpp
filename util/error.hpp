/****************************************************************
* Error handling functionality
****************************************************************/
#pragma once

#include "non-copyable.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace util {

/* Error: this struct  is  essentially  just  a  wrapper around a
 * string  for  the purpose of distinguishing it type-wise from a
 * string and to prevent implicit construction. */
struct Error {
    explicit Error( std::string_view sv ) : msg( sv ) {}
    bool operator==( Error const& rhs ) const
        { return rhs.msg == msg; }
    std::string msg;
};

// This  is  a  general  type that can be used to return a result
// that may have failed. Its value can be either the return value
// or failure, which then contains an error message.
template<typename PayloadT>
using Result = std::variant<PayloadT, Error>;

// For convenience
template<typename T>
std::ostream& operator<<( std::ostream&    out,
                          Result<T> const& res ) {
    if( std::holds_alternative<Error>( res ) ) {
        std::string msg = std::get<Error>( res ).msg;
        return (out << "<error>: " << msg);
    }
    return (out << std::get<T>( res ));
}

} // namespace util
