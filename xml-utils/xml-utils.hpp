#pragma once

#include "macros.hpp"
#include "pugixml.hpp"

#include <experimental/filesystem>
#include <string>
#include <optional>
#include <type_traits>
#include <tuple>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace xml {

using KeyVal    = std::tuple<std::string, std::string>;
using XPathVars = std::vector<KeyVal>;

pugi::xpath_node_set xpath( std::string           cmd,
                            pugi::xml_node const& node,
                            std::vector<KeyVal>   vars );

template<typename T>
std::optional<T> attribute( pugi::xml_node const& node,
                            char const*           name ) {
    if( auto attr = node.attribute( name ).as_string( NULL ); attr ) {
        if constexpr( std::is_convertible_v<decltype( attr ),T> )
            return T( attr );
        else {
            T res; std::istringstream ss( attr );
            // Ensure it parsed successfully and exhausted the string.
            if( (ss >> res) && ss && ss.eof() )
                return res;
        }
    }
    return std::nullopt;
}

struct err_location {
    int line, pos;
};

err_location offset_to_line( int offset, fs::path file );

void parse( pugi::xml_document& doc, fs::path file );

} // namespace xml