/****************************************************************
* Utility Functions for Use with PugiXml
****************************************************************/
#include "macros.hpp"
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <fstream>

using namespace std;

namespace xml {

// Run  an  xpath  command with a list of key/val pairs to substi-
// tute into the query.
pugi::xpath_node_set xpath( std::string           cmd,
                            pugi::xml_node const& node,
                            std::vector<KeyVal>   vars ) {
    pugi::xpath_variable_set xvars;
    for( auto& [k,v] : vars )
        ASSERT_( xvars.set( k.c_str(), v.c_str() ) );
    
    pugi::xpath_query query( cmd.c_str(), &xvars );
    return query.evaluate_node_set( node );
}

// Convert a character offset in the file to a line number
// and line-offset pair.
err_location offset_to_line( int offset, fs::path file ) {
    ifstream in( file );
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

void parse( pugi::xml_document& doc, fs::path file ) {

    ifstream in( file );
    ASSERT_( in.good() );

    pugi::xml_parse_result res = doc.load( in );

    if( res )
        return;

    ostringstream out;

    out << "failed to parse xml file " << file;
    if( res.offset > 0 ) {
		auto [line, pos] = offset_to_line( res.offset, file );
		out << " on line " << line << ", position " << pos;
	}
        
	out << ": " << res.description();

    throw runtime_error( out.str() );
}

/****************************************************************
* XPath Wrappers
****************************************************************/
vector<string>
attributes( pugi::xml_document const& doc,
            char const*               x_path,
            XPathVars const&          vars,
            bool                      allow_empty ) {

    string path( x_path );
    vector<string> res;
    for( auto n : xpath( path, doc, vars ) ) {
        ASSERT_( n.attribute() );
        string value( n.attribute().value() );
        if( !allow_empty ) { ASSERT_( !value.empty() ); }
        res.push_back( value );
    }
    return move( res );
}

vector<string>
texts( pugi::xml_document const& doc,
       const char*               x_path,
       xml::XPathVars const&     vars,
       bool                      allow_empty,
       bool                      strip ) {

    string path( x_path );
    vector<string> res;
    for( auto n : xml::xpath( path, doc, vars ) ) {
        ASSERT_( n.node() );
        string_view sv( n.node().text().get() );
        if( !allow_empty ) ASSERT_( !sv.empty() );
        if( strip) sv = util::strip( sv );
        res.push_back( string( sv ) );
    }
    return move( res );
}

// Just likes texts() but will assert that there is precisely one
// result (no more no less) and throw otherwise.
string
text( pugi::xml_document const& doc,
      const char*               x_path,
      xml::XPathVars const&     vars,
      bool                      allow_empty,
      bool                      strip ) {

    auto res( texts( doc, x_path, vars, allow_empty, strip ) );
    ASSERT( res.size() == 1, "number of results for xpath:"
                          << endl << quoted( x_path ) << endl
                          << "is " << res.size()
                          << ", but must be 1." );
    return move( res[0] );
}

} // namespace xml
