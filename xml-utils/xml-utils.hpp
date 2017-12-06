/****************************************************************
* Utility Functions for Use with PugiXml
****************************************************************/
#pragma once

#include "macros.hpp"
#include "pugixml.hpp"
#include "types.hpp"

#include <experimental/filesystem>
#include <string>
#include <optional>
#include <string>
#include <type_traits>
#include <tuple>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace xml {

// These are the tyeps used for conveying values for variables in
// xpath queries.
using XPKeyVal  = std::tuple<std::string, std::string>;
using XPathVars = std::vector<XPKeyVal>;

pugi::xpath_node_set xpath( std::string           cmd,
                            pugi::xml_node const& node,
                            XPathVars             vars = {} );

template<typename T>
std::optional<T> attribute( pugi::xml_node const& node,
                            char const*           name ) {
    if( auto attr = node.attribute( name ).as_string( NULL ); attr ) {
        if constexpr( std::is_convertible_v<decltype( attr ),T> )
            return T( attr );
        else {
            T res; std::istringstream ss( attr );
            // Ensure it parsed successfully  and  exhausted  the
            // string.
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

/****************************************************************
* XPath Wrappers
****************************************************************/
StrVec attr( pugi::xml_document const& doc,
             char const*               x_path,
             XPathVars const&          vars,
             bool                      allow_empty = true );

StrVec texts( pugi::xml_document const& doc,
              const char*               x_path,
              xml::XPathVars const&     vars,
              bool                      allow_empty = true,
              bool                      strip       = true );

// Just likes texts() but will  return  nullopt  if the number of
// results is zero and will throw  if number of results is larger
// than one.
OptStr text( pugi::xml_document const& doc,
             const char*               x_path,
             xml::XPathVars const&     vars,
             bool                      allow_empty = true,
             bool                      strip       = true );

} // namespace xml
