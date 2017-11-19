/****************************************************************
* Test driver for cpp libraries
****************************************************************/
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{
try {

    cout << endl;

    pugi::xml_document doc;

    // path may be relative to bin folder
    xml::parse( doc, "../xml-utils/samples/simple.xml" );

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

    auto path( "//*[name()=$start][@color=$color1 or contains(@color,$color2)]" );
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
    return 0;

} catch( exception const& e ) {
    cerr << "exception: " << e.what() << endl;
    return 1;
}
}
