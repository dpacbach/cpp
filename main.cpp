/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

namespace xpaths {


char const* cl_compiles = "/Project/ItemGroup/ClCompile/@Include";
char const* cl_includes = "/Project/ItemGroup/ClInclude/@Include";

char const* search_paths = "/Project/ItemDefinitionGroup[contains(@Condition,$platform)]/ClCompile/AdditionalIncludeDirectories";
}

vector<string> attributes( pugi::xml_document const& doc,
                           const char*               xpath,
                           xml::XPathVars const&     vars,
                           bool                      allow_empty = true ) {
    string path( xpath );
    vector<string> res;
    for( auto n : xml::xpath( path, doc, vars ) ) {
        ASSERT_( n.attribute() );
        string value( n.attribute().value() );
        if( !allow_empty ) { ASSERT_( !value.empty() ); }
        res.push_back( value );
    }
    return res;
}

vector<string> texts( pugi::xml_document const& doc,
                      const char*               xpath,
                      xml::XPathVars const&     vars,
                      bool                      allow_empty = true,
                      bool                      strip = true ) {
    string path( xpath );
    vector<string> res;
    for( auto n : xml::xpath( path, doc, vars ) ) {
        ASSERT_( n.node() );
        string_view sv( n.node().text().get() );
        if( !allow_empty ) ASSERT_( !sv.empty() );
        if( strip) sv = util::strip( sv );
        res.push_back( string( sv ) );
    }
    return res;
}

vector<string> cl_compiles( pugi::xml_document const& doc ) {
    return attributes( doc, xpaths::cl_compiles, {}, false );
}

vector<string> cl_includes( pugi::xml_document const& doc ) {
    return attributes( doc, xpaths::cl_includes, {}, false );
}

vector<string> search_paths( pugi::xml_document const& doc,
                             string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    return texts( doc, xpaths::search_paths, vars, true, true );
    // TODO: need to split on semi colon and drop last part
}

int main()
{
try {

    cout << endl;

    pugi::xml_document doc;

    // path may be relative to bin folder
    xml::parse( doc, "../xml-utils/samples/pugixml_vs2013.vcxproj" );

    for( auto const& s : search_paths( doc, "Debug|x64" ) )
        cout << s << endl;
    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
}

/*
    cout << "char size: " << sizeof( pugi::char_t ) << endl;

    auto node = doc.child( "root_node" ).child( "child" );

    cout << "empty: " << empty( node ) << endl;

    cout << node.attribute( "color" ).value() << endl;
    
    cout << node.path() << endl;

    char const* attr_name = "type";
    if( auto attr = xml::attribute<int>( node, attr_name ); attr )
        cout << attr_name << ": " << attr.value() << endl;
    else
        cout << "error getting attribute " << attr_name << endl;

    auto root = doc.child( "root_node" );

    auto purple = root.find_node( []( auto const& node ) {
        string color( node.attribute( "color" ).value() );
        return (color == "purple");
    });

    if( purple )
        cout << purple.path() << endl;
        //purple.print( cout );
    else
        cout << "not found." << endl;

    cout << util::strip( purple.text().get() ) << endl;

    auto path( "*[name()=$start][@color=$color1 or contains(@color,$color2)]" );
    xml::XPathVars vars{
        { "start",  "child"  },
        { "color1", "blue"   },
        { "color2", "e"      },
    };

    cout << "Selected nodes:" << endl;
    for( auto n: xml::xpath( path, doc, vars ) ) {
        if( n.node() )
            cout << "    " << n.node().path() << endl;
        else if( n.attribute() )
            cout << "    @" << n.attribute().value() << endl;
    }

 */
