/****************************************************************
* Utility Functions for Use with PugiXml
****************************************************************/
#pragma once

#include "macros.hpp"
#include "pugixml.hpp"
#include "types.hpp"

#include <string>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <tuple>
#include <vector>

namespace xml {

namespace impl {

// This  is  like the `filter` function for an xml tree: it takes
// an xml root node and function, and returns a list of all nodes
// for  which the function returns true. The pugixml library does
// generic xml tree traversal by taking a user-supplied  subclass
// of xml_tree_walker. The for_each method  is overriden and will
// be  called  on  each node. In our case, we just call the given
// function  on  the  node and keep a list of all those for which
// the function returns true.
template<typename Func>
struct filter_walker : public pugi::xml_tree_walker {

    filter_walker( Func f ) : keep( f ) {}

    virtual bool for_each( pugi::xml_node& node ) override {
        if( keep( node ) )
            found.push_back( node );
        return true;
    }

    Func keep;
    std::vector<pugi::xml_node> found;
};

} // namespace impl

// Traverse an entire node tree and return a vector of all  nodes
// for which the supplied function returns true. The  function  f
// shall  take  an  xml_node  either by value or by reference and
// shall return true or false, true meaning that the node  should
// be selected.
template<typename Func>
std::vector<pugi::xml_node> filter( pugi::xml_node n, Func f ) {
    impl::filter_walker fw( f );
    n.traverse( fw );
    return move( fw.found );
}

// These are the tyeps used for conveying values for variables in
// xpath queries.
using XPKeyVal  = std::tuple<std::string, std::string>;
using XPathVars = std::vector<XPKeyVal>;

pugi::xpath_node_set xpath( std::string_view      cmd,
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

void parse( pugi::xml_document& doc, fs::path    const& file );
void parse( pugi::xml_document& doc, std::string const& s );

// This variant allows taking an error  message that will be used
// as a prefix in the event of a parsing error.
void parse( pugi::xml_document& doc,
            std::istream&       in,
            std::string const&  err_msg = "" );

/****************************************************************
* XPath Wrappers
****************************************************************/
StrVec attrs( pugi::xml_node const& node,
              char const*           x_path,
              XPathVars const&      vars = {},
              bool                  allow_empty = true );

// Same as above  by  enforces  that  there  be  only  one result.
OptStr attr( pugi::xml_node const& node,
             char const*           x_path,
             XPathVars const&      vars = {},
             bool                  allow_empty = true );

StrVec texts( pugi::xml_node const& node,
              const char*           x_path,
              xml::XPathVars const& vars = {},
              bool                  allow_empty = true,
              bool                  strip       = true );

// Just likes texts() but will  return  nullopt  if the number of
// results is zero and will throw  if number of results is larger
// than one.
OptStr text( pugi::xml_node const& doc,
             const char*           x_path,
             xml::XPathVars const& vars = {},
             bool                  allow_empty = true,
             bool                  strip       = true );

} // namespace xml
