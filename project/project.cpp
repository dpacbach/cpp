/****************************************************************
* Project & Solution File Parsers
****************************************************************/
#include "macros.hpp"
#include "project.hpp"
#include "string-util.hpp"
#include "xml-utils.hpp"

#include <algorithm>
#include <sstream>

using namespace std;

namespace project {

namespace xpaths {

char const* cl_compiles = "/Project/ItemGroup/ClCompile/@Include";
char const* cl_includes = "/Project/ItemGroup/ClInclude/@Include";

char const* search_paths =
    " /descendant-or-self::node()[              "
    "     contains(@Condition,$platform)        "
    " ]                                         "
    " /descendant-or-self::node()[              "
    "     name()='AdditionalIncludeDirectories' "
    " ]                                         ";

char const* int_dir =
    " /descendant-or-self::node()[              "
    "     contains(@Condition,$platform)        "
    " ]                                         "
    " /descendant-or-self::node()[              "
    "     name()='IntDir'                       "
    " ]                                         ";

}

vector<fs::path> cl_compiles( pugi::xml_document const& doc ) {
    auto res( xml::attributes(
                    doc, xpaths::cl_compiles, {}, false ) );
    return util::to_paths( res );
}

vector<fs::path> cl_includes( pugi::xml_document const& doc ) {
    auto res( xml::attributes(
                    doc, xpaths::cl_includes, {}, false ) );
    return util::to_paths( res );
}

vector<fs::path> search_paths( pugi::xml_document const& doc,
                               string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    auto paths = xml::texts(
            doc, xpaths::search_paths, vars, true, true );
    // For a given platform, we must have precisely one resultant
    // (possibly  semicolon  separated)  list  of  search   paths.
    ASSERT( paths.size() == 1, "size is " << paths.size() );
    auto res_win = util::split_strip( paths[0], ';' );
    vector<fs::path> res( res_win.size() );
    transform( begin( res_win ), end( res_win ), begin( res ),
               util::to_path );
    return move( res );
}

fs::path int_dir( pugi::xml_document const& doc,
                string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    auto dirs = xml::texts(
            doc, xpaths::int_dir, vars, false, true );
    // For a given platform  we  must  have  precisely one result.
    ASSERT( dirs.size() == 1, "size is " << dirs.size() );
    return fs::path( dirs[0] );
}

Project::Project( vector<fs::path>&& cl_includes,
                  vector<fs::path>&& cl_compiles,
                  vector<fs::path>&& search_paths,
                  fs::path&&         int_dir )
  : cl_includes  ( move( cl_includes  ) ),
    cl_compiles  ( move( cl_compiles  ) ),
    search_paths ( move( search_paths ) ),
    int_dir      ( move( int_dir      ) )
{ }

auto read( fs::path file, string_view platform ) -> Project {

    pugi::xml_document doc;

    // path may be relative to bin folder
    xml::parse( doc, file );

    return Project(
        cl_includes( doc ),
        cl_compiles( doc ),
        search_paths( doc, platform ),
        int_dir( doc, platform )
    );
}

string Project::to_string() const {

    ostringstream oss;

    auto print = [&oss]( auto const& s ) {
            oss << "  | " <<  quoted( string( s ) ) << endl;
    };

    auto print_list = [&]( auto const& v ) {
        for( auto const& s : v )
            print( s );
    };

    oss << "Search paths: " << endl;
    print_list( search_paths );
    oss << "Cl Compiles: " << endl;
    print_list( cl_compiles );
    oss << "Cl Includes: " << endl;
    print_list( cl_includes );
    oss << "IntDir: " << endl;
    print( string( int_dir ) );

    return oss.str();
}

ostream& operator<<( ostream& out, Project const& p ) {
    return (out << p.to_string());
}

} // namespace project
