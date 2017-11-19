/****************************************************************
* String utilities
****************************************************************/
#pragma once

#include <string_view>

namespace util {

// Strip all blank space off of a string view and return
// a new one.
std::string_view strip( std::string_view sv );

}
