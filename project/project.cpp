/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#include "macros.hpp"
#include "project.hpp"
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <algorithm>

using namespace std;

namespace project {

namespace xpaths {

char const* cl_compiles = "/Project/ItemGroup/ClCompile/@Include";
char const* cl_includes = "/Project/ItemGroup/ClInclude/@Include";

char const* search_paths = "/Project"
                           "/ItemDefinitionGroup[contains(@Condition,$platform)]"
                           "/ClCompile"
                           "/AdditionalIncludeDirectories";
}

vector<string> cl_compiles( pugi::xml_document const& doc ) {
    auto res( xml::attributes(
                    doc, xpaths::cl_compiles, {}, false ) );
    transform( begin( res ), end( res ), begin( res ),
               util::fwd_slashes );
    return move( res );
}

vector<string> cl_includes( pugi::xml_document const& doc ) {
    auto res( xml::attributes(
                    doc, xpaths::cl_includes, {}, false ) );
    transform( begin( res ), end( res ), begin( res ),
               util::fwd_slashes );
    return move( res );
}

vector<string> search_paths( pugi::xml_document const& doc,
                             string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    auto paths = xml::texts(
            doc, xpaths::search_paths, vars, true, true );
    if( paths.empty() ) return {};
    // For  a  given  platform, if we have a non-empty result, we
    // must  only  have  one  resultant  (possibly semicolon sepa-
    // rated) list of search paths.
    ASSERT_( paths.size() == 1 );
    auto res_win = util::split_strip( paths[0], ';' );
    vector<string> res( res_win.size() );
    transform( begin( res_win ), end( res_win ), begin( res ),
               util::fwd_slashes );
    return move( res );
}

Project::Project( vector<string>&& cl_includes,
                  vector<string>&& cl_compiles,
                  vector<string>&& search_paths )
  : cl_includes( move( cl_includes ) ),
    cl_compiles( move( cl_compiles ) ),
    search_paths( move( search_paths ) )
{ }

Project read( fs::path file, string_view platform ) {

    pugi::xml_document doc;

    // path may be relative to bin folder
    xml::parse( doc, file );

    return move( Project(
        cl_includes( doc ),
        cl_compiles( doc ),
        search_paths( doc, platform )
    ) );
}

ostream& operator<<( ostream& out, Project const& p ) {

    auto print_list = [&out]( auto const& v ){
        for( auto const& s : v )
            out << "  " <<  quoted( s ) << endl;
    };

    out << endl << "Search paths: " << endl;
    print_list( p.search_paths );
    out << endl << "Cl Compiles: " << endl;
    print_list( p.cl_compiles );
    out << endl << "Cl Includes: " << endl;
    print_list( p.cl_includes );

    return out;
}

} // namespace project
