/****************************************************************
* Adding and Removing from Project Files
****************************************************************/
#include "add-remove.hpp"

#include "fs.hpp"
#include "logger.hpp"
#include "string-util.hpp"
#include "xml-util.hpp"

using namespace std;

namespace project {

namespace {

string to_project( fs::path const& proj_file,
                   fs::path const& src_file ) {

    auto proj_folder =
        util::absnormpath( proj_file ).parent_path();

    auto src = util::back_slashes(
        util::lexically_relative(
            util::absnormpath( src_file ),
            proj_folder
        ).string()
    );
    return src;
}

fs::path from_project( fs::path const& proj_file,
                       string   const& src_win ) {

    auto src = fs::path( util::fwd_slashes( src_win ) );

    auto proj_folder =
        util::absnormpath( proj_file ).parent_path();

    return util::lexically_normal( proj_folder/src);
}

bool match_src( fs::path const& proj_file,
                fs::path const& p1,
                string   const& p2_win ) {

    auto p1_abs = p1;

    if( !p1_abs.is_absolute() )
        p1_abs = util::lexically_normal( fs::current_path()/p1 );

    auto p2 = from_project( proj_file, p2_win );

    return util::iequals( p1_abs.string(), p2.string() );
}

vector<pugi::xml_node> find_src( pugi::xml_node const& node,
                                 fs::path       const& proj_file,
                                 fs::path       const& src ) {

    return xml::filter( node, [&]( pugi::xml_node n ) {

        auto Include = xml::attr( n, "@Include" );
        return Include &&
               string( n.name() ) == "ClCompile" &&
               match_src( proj_file, src, *Include );

    } );
}

} // anonymous namespace

// Open the project file and insert the given  source  file  into
// the list of ClCompile's of the project. Note, slashes will  al-
// ways  be converted to back slashes when writing paths into the
// project file, regardless of platform.  The  source file, if it
// is  a  relative  path, is interpreted as being relative to CwD
// (not relative to the  project  file  location).  If the source
// file already exists in the project file then an exception will
// be thrown.
void add_2_vcxproj( fs::path const& proj_file,
                    fs::path const& src_file ) {

    pugi::xml_document doc; xml::parse( doc, proj_file );

    ASSERT( find_src( doc, proj_file, src_file ).empty(),
           "Project file " << proj_file << " already contains "
           "source file "  << src_file );

    auto ItemGroup = xml::xpath( "//ItemGroup[ClCompile]", doc );

    pugi::xml_node parent = ItemGroup.empty()
                          ? xml::node( doc, "/Project" )
                            .append_child( "ItemGroup" )
                          : ItemGroup[0].node();

    parent.append_child( "ClCompile" )
          .append_attribute( "Include" ) =
           to_project( proj_file, src_file ).c_str();

    auto name = proj_file.string();
    ASSERT( doc.save_file( name.c_str(), "  " ), "failed to "
            "save document to file " << name );
}

// Open  the  filters  file and insert the given source file into
// it. First we search the filters  file for any other files that
// are  in the same folder as the given one and, if there are, we
// will obtain the name of the filter in that way by choosing the
// first filter that we encounter that contains at least one file
// in the same folder  as  src_file.  Otherwise,  we will use the
// first filter that we find. The source file, if it  is  a  rela-
// tive path, is interpreted as being relative to CWD  (not  rela-
// tive to the project file location). If the source file already
// exists in the filters file  then  an  exception will be thrown.
void add_2_filters( fs::path const& filters_file,
                    fs::path const& src_file ) {
    (void)filters_file;
    (void)src_file;
}

// This  is simply for convenience; it will call add_2_project on
// the given project file,  then  will  construct  a filters file
// path by appending ".filters" to the  end  of the name and will
// then call add_2_filters on the filters file.
void add_2_project( fs::path const& proj_file,
                    fs::path const& src_file ) {
    add_2_vcxproj( proj_file, src_file );
    auto filters = proj_file;
    filters += ".filters";
    add_2_filters( filters, src_file );
}

// Open the given file, which could be either a project file or a
// filters file, and remove any ClCompile elements from within it
// that reference the given source file.  Note  that  the  source
// file,  if it is a relative path, is interpreted as a path rela-
// tive  to  CWD.  The source file path and the various ClCompile
// paths  encountered  in  the file will be normalized before per-
// forming  a case-insensitive comparison to find a match. If the
// source file is not found in the file then an exception will be
// thrown.
void rm_from( fs::path const& file,
              fs::path const& src_file ) {
    (void)file;
    (void)src_file;
}


} // namespace project
