/****************************************************************
* Network Utilities
****************************************************************/
#pragma once

#include <string>
#include <string_view>

namespace net {

// URL-encode a string.
std::string url_encode( std::string_view in );

} // namespace net
