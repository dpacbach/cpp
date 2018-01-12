/****************************************************************
* Utility Functions for Use with PugiXml
****************************************************************/
#include "macros.hpp"
#include "string-util.hpp"
#include "xml-util.hpp"

#include <fstream>

using namespace std;

namespace xml {

// Run  an  xpath  command with a list of key/val pairs to substi-
// tute into the query.
pugi::xpath_node_set xpath( string_view           cmd,
                            pugi::xml_node const& node,
                            XPathVars             vars ) {
    pugi::xpath_variable_set xvars;
    for( auto& [k,v] : vars )
        ASSERT( xvars.set( k.c_str(), v.c_str() ),
               "error accepting query substitution variables" );

    pugi::xpath_query query( string( cmd ).c_str(), &xvars );
    return query.evaluate_node_set( node );
}

// Convert a character offset in the file to a line number
// and line-offset pair.
err_location offset_to_line( int offset, istream& in ) {

    int line = 1, accum = 0;
    do {
        string s;
        getline( in, s );
        // Need to add one for newline
        auto len = 1 + s.length();
        if( accum + int( len ) > offset )
            break;
        accum += len;
        ++line;
    }   while( in.good() );
    return { line, offset-accum };
}

void throw_parse_error( pugi::xml_parse_result const& res,
                        istream&                      in,
                        string const&                 msg ) {

    ostringstream out;
    out << msg;

    if( res.offset > 0 ) {
		auto [line, pos] = offset_to_line( res.offset, in );
		out << " on line " << line << ", pos " << pos;
	}

	out << ": " << res.description();

    throw runtime_error( out.str() );
}

void parse( pugi::xml_document& doc, fs::path const& file ) {

    ifstream in( file.string() );
    ASSERT( in.good(), "failed to read file " << file );

    pugi::xml_parse_result res = doc.load( in );

    if( res )
        return;

    // Rewind input file so that we can traverse it from  the  be-
    // ginning again to find line of error.
    in.clear(); in.seekg( 0 );

    throw_parse_error( res, in,
                       string( "failed to parse xml file " ) +
                       util::to_string( file ) );
}

void parse( pugi::xml_document& doc, string const& s ) {

    pugi::xml_parse_result res = doc.load_string( s.c_str() );

    if( res )
        return;

    istringstream in( s );
    throw_parse_error( res, in, "failed to parse xml string" );
}

/****************************************************************
* XPath Wrappers
****************************************************************/
StrVec attrs( pugi::xml_node const& node,
              char const*           x_path,
              XPathVars const&      vars,
              bool                  allow_empty ) {

    vector<string> res;
    for( auto n : xpath( x_path, node, vars ) ) {
        ASSERT( n.attribute(), "xpath query for attribute "
               "yielded something that was not an attribute. "
               "Query String: " << x_path );
        string value( n.attribute().value() );
        if( !allow_empty ) {
            ASSERT( !value.empty(), "xpath query for attribute "
                   "yielded empty result, when it should not "
                   "have. Query string: " << x_path );
        }
        res.push_back( value );
    }
    return res;
}

// Same as above  by  enforces  that  there  be  only  one result.
OptStr attr( pugi::xml_node const& node,
             char const*           x_path,
             XPathVars const&      vars,
             bool                  allow_empty ) {
    auto res = attrs( node, x_path, vars, allow_empty );
    if( res.size() == 1 )
        return res[0];
    return nullopt;
}

StrVec texts( pugi::xml_node const& node,
              const char*           x_path,
              xml::XPathVars const& vars,
              bool                  allow_empty,
              bool                  strip ) {

    vector<string> res;
    for( auto n : xml::xpath( x_path, node, vars ) ) {
        ASSERT( n.node(), "xpath query for texts of nodes did "
               "find one or more nodes.  Query: " << x_path );
        string_view sv( n.node().text().get() );
        if( !allow_empty ) {
            ASSERT( !sv.empty(), "xpath query for texts found "
                   "any empty string.  Query: " << x_path );
        }
        if( strip) sv = util::strip( sv );
        res.emplace_back( sv );
    }
    return res;
}

// Just likes texts() but will  return  nullopt  if the number of
// results is not precisely one.
OptStr text( pugi::xml_node const& node,
             const char*           x_path,
             xml::XPathVars const& vars,
             bool                  allow_empty,
             bool                  strip ) {

    auto res( texts( node, x_path, vars, allow_empty, strip ) );
    if( res.size() != 1 )
        return nullopt;
    return OptStr( move( res[0] ) );
}

} // namespace xml
