/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#include "project.hpp"
#include "xml-utils.hpp"
#include "string-util.hpp"
#include "util.hpp"

#include <algorithm>

using namespace std;

namespace project {

namespace {

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

char const* out_dir =
    " /descendant-or-self::node()[              "
    "     contains(@Condition,$platform)        "
    " ]                                         "
    " /descendant-or-self::node()[              "
    "     name()='OutDir'                       "
    " ]                                         ";

char const* project_name = "//ProjectName";
char const* project_guid = "//ProjectGuid";

char const* target_name =
    " /descendant-or-self::node()[              "
    "     contains(@Condition,$platform)        "
    " ]                                         "
    " /descendant-or-self::node()[              "
    "     name()='TargetName'                   "
    " ]                                         ";

char const* target_ext =
    " /descendant-or-self::node()[              "
    "     contains(@Condition,$platform)        "
    " ]                                         "
    " /descendant-or-self::node()[              "
    "     name()='TargetExt'                    "
    " ]                                         ";

}

vector<string> cl_compiles( pugi::xml_document const& doc ) {
    return xml::attr( doc, xpaths::cl_compiles, {}, false );
}

vector<string> cl_includes( pugi::xml_document const& doc ) {
    return xml::attr( doc, xpaths::cl_includes, {}, false );
}

vector<string> search_paths( pugi::xml_document const& doc,
                             string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    // For a given platform, we must have precisely one resultant
    // (possibly  semicolon  separated)  list  of  search   paths.
    auto path = xml::text(
            doc, xpaths::search_paths, vars, true, true );
    if( !path )
        return {};
    auto res = util::split_strip( *path, ';' );
    auto is_var = L( util::contains( _, "%(" ) );
    util::remove_if( res, is_var );
    return util::to_strings( res );
}

string int_dir( pugi::xml_document const& doc,
                string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    auto dir = xml::text(
                   doc, xpaths::int_dir, vars, false, true );
    ASSERT( dir, "failed to find precisely one value for "
                 "IntDir for the " << platform << " platform. "
                 "There may be zero, or too many." );
    return *dir;
}

string out_dir( pugi::xml_document const& doc,
                string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    auto dir = xml::text(
                   doc, xpaths::out_dir, vars, false, true );
    ASSERT( dir, "failed to find precisely one value for "
                 "OutDir for the " << platform << " platform. "
                 "There may be zero, or too many." );
    return *dir;
}

string project_name( pugi::xml_document const& doc ) {
    auto name = xml::text(
                    doc, xpaths::project_name, {}, false, true );
    ASSERT( name, "failed to find precisely one value for "
                  "ProjectName. There may be zero, or too many." );
    return move( *name );
}

optional<string> target_name( pugi::xml_document const& doc,
                              string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    return xml::text(
                doc, xpaths::target_name, vars, true, true );
}

optional<string> target_ext( pugi::xml_document const& doc,
                             string_view               platform ) {
    xml::XPathVars vars{ { "platform", string( platform ) } };
    return xml::text(
                doc, xpaths::target_ext, vars, true, true );
}

string uuid( pugi::xml_document const& doc ) {
    auto res( xml::text(
                doc, xpaths::project_guid, {}, false, true ) );
    ASSERT( res, "failed to find precisely one value for "
                 "ProjectGuid. There may be zero, or too many." );
    string_view sv( *res );
    ASSERT( sv[0] == '{' && sv[sv.size()-1] == '}',
                 "failed to find uuid surrounded in { }." );
    // Remove { } from around the UUID.
    return string( sv.substr( 1, sv.size()-2 ) );
}

// This will parse the XML file and  then traverse it to find the
// data  necessary to populate the ProjectAttr structure. However,
// it will perform only the  minimum  amount of processing on the
// data necessary to populate the ProjectAttr struct.
ProjectAttr parse( fs::path const& file,
                   string_view     platform ) {

    pugi::xml_document doc;

    // path may be relative to bin folder
    xml::parse( doc, file );

    // Need to flip slashes  here  while  they  are still strings,
    // since  once they are converted to paths there doesn't seem
    // to be a way to do it.
    auto fwd  = L(       fs::path( util::fwd_slashes( _ ) ) );
    auto fwds = L( util::to_paths( util::fwd_slashes( _ ) ) );

    return { {}, // ProjectAttr base
        move( fwds( cl_includes ( doc           ) ) ),
        move( fwds( cl_compiles ( doc           ) ) ),
        move( fwds( search_paths( doc, platform ) ) ),
        move( fwd ( int_dir     ( doc, platform ) ) ),
        move( fwd ( out_dir     ( doc, platform ) ) ),
        move(       project_name( doc           ) ),
        move(       target_name ( doc, platform ) ),
        move(       target_ext  ( doc, platform ) ),
        move(       uuid        ( doc           ) )
    };
}

} // anonymous namespace

Project::Project( ProjectAttr&& pa )
  : m_attr( move( pa ) )
{}

Project Project::read( fs::path const& file,
                       string_view     platform,
                       fs::path const& base ) {

    auto p = parse( file, platform );

    auto abs_dir = util::absnormpath( file ).parent_path();

    auto use_rel = !base.empty();

    auto abs = [&]( auto const& p ) {
        auto res = util::lexically_normal( abs_dir / p );
        if( use_rel )
            res = util::lexically_relative( res, base );
        return res;
    };

    auto abs_vec = [&]( auto& v ) {
        transform( begin( v ), end( v ), begin( v ), abs );
    };

    abs_vec( p.cl_includes  );
    abs_vec( p.cl_compiles  );
    abs_vec( p.search_paths );
    p.int_dir = abs( p.int_dir );
    p.out_dir = abs( p.out_dir );

    return Project{ {
        {},   // ProjectAttr base
        move( p.cl_includes  ),
        move( p.cl_compiles  ),
        move( p.search_paths ),
        move( p.int_dir      ),
        move( p.out_dir      ),
        move( p.project_name ),
        move( p.target_name  ),
        move( p.target_ext   ),
        move( p.uuid         )
    } };
}

ostream& operator<<( ostream& out, Project const& p ) {
    return (out << p.attr());
}

} // namespace project
