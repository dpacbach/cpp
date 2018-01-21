/****************************************************************
* Unit tests for XML functionality
****************************************************************/
#include "common-test.hpp"

#include "string-util.hpp"
#include "xml-util.hpp"

using namespace std;
using namespace pugi;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( xml_filter )
{
    xml_document doc;
    xml::parse( doc, data_common/"status.xml" );

    auto f1 = []( xml_node& n ) {
        string attr( n.attribute( "item" ).value() );
        return (attr == "conflicted");
    };

    auto v1 = xml::filter( doc, f1 );
    EQUALS( v1.size(), 1 );

    auto a = xml::text( v1[0], "commit/author" );
    TRUE_( a );
    EQUALS( *a, "jack" );

    auto f2 = []( xml_node& n ) {
        string attr( n.attribute( "path" ).value() );
        return (util::ends_with( attr, ".py" ));
    };

    auto v2 = xml::filter( doc, f2 );
    EQUALS( v2.size(), 3 );
    EQUALS( string( v2[0].attribute( "path" ).value() ), "test.py"         );
    EQUALS( string( v2[1].attribute( "path" ).value() ), "python.py"       );
    EQUALS( string( v2[2].attribute( "path" ).value() ), "another_file.py" );
}

} // namespace testing
