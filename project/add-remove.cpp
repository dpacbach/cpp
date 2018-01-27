/****************************************************************
* Adding and Removing from Project Files
****************************************************************/
#include "add-remove.hpp"

#include "fs.hpp"
#include "logger.hpp"
#include "string-util.hpp"
#include "xml-util.hpp"

using namespace std;

using pugi::xml_document;
using pugi::xml_node;

namespace project {

namespace {

// Takes a project file which must exist and, if relative, is  as-
// sumed relative to CWD. Then takes a source file which may  not
// exist  and,  if  relative, is assumed relative to CWD. It will
// make the source file relative to  the  folder  containing  the
// project file and convert slashes to backslashes and return the
// result as a string. The reason for the string result  is  that,
// on some platforms, it would not work to convert  a  path  with
// backslashes to an fs::path.
string to_prj( fs::path const& prj, fs::path const& src ) {

    return util::back_slashes(
        util::lexically_relative(
            util::lexically_absolute( src ),
            util::absnormpath( prj ).parent_path()
        ).string()
    );
}

// Takes a project file which must exist and, if relative, is  as-
// sumed relative to CWD. Then takes a string whose contents  rep-
// resent a source file path as  it would appear inside a project
// file (i.e., possibly absolute, or possibly relative to project
// file location, and with backslashes). The  return  is  the  ab-
// solute  (native)  path to the source file, which may not exist,
// on the filesystem.
fs::path from_prj( fs::path const& prj, string const& src_win ) {

    auto src = fs::path( util::fwd_slashes( src_win ) );
    auto prj_dir = util::absnormpath( prj ).parent_path();
    return util::lexically_normal( util::slash( prj_dir, src ) );
}

// Given a project (represented by the given project file), would
// the  string  p2, if it were inside the project file, reference
// the file p1? p1 may not exist and, if it is  relative,  is  as-
// sumed  relative  to CWD. p2 can be a literal string taken from
// within  a  project file, i.e., may contain backslashes, may be
// absolute  or  relative, and if relative is assumed relative to
// location of project folder.
bool match_src( fs::path const& prj,
                fs::path const& p1,
                string   const& p2 ) {

    return util::path_equals( util::lexically_absolute( p1 ),
                              from_prj( prj, p2 ),
                              util::CaseSensitive::NO );
}

// Same as match_src  except  it  cares  only  about matching the
// parent folders of the files in question.
bool match_src_dir( fs::path const& prj,
                    fs::path const& p1,
                    string   const& p2 ) {

    return util::path_equals(
            util::lexically_absolute( p1 ).parent_path(),
            from_prj( prj, p2 ).parent_path(),
            util::CaseSensitive::NO
    );
}

// Starting from the given node, traverse the tree and look for a
// reference to a  source  file  that  corresponds  to  the given
// source  file  in the sense that the normalized, absolute paths
// should match in a case-insensitive way.
vector<xml_node> find_src( xml_node const& node,
                           fs::path const& prj,
                           fs::path const& src ) {

    return xml::filter( node, [&]( xml_node n ) {
        auto Inc = xml::attr( n, "@Include" );
        return Inc && string( n.name() ) == "ClCompile" &&
                      match_src( prj, src, *Inc );
    } );
}

// Starting from the given node, traverse the tree and look for a
// reference to a source file that  resides in the same folder as
// the  given  source file in the sense that the absolute, normal-
// ized paths match in a case-insensitive fashion.
vector<xml_node> find_src_dir( xml_node const& node,
                               fs::path const& prj,
                               fs::path const& src ) {

    return xml::filter( node, [&]( xml_node n ) {
        auto Inc = xml::attr( n, "@Include" );
        return Inc && string( n.name() ) == "ClCompile" &&
                      match_src_dir( prj, src, *Inc );
    } );
}

} // anonymous namespace

// Open the project file and insert the given  source  file  into
// the list of ClCompile's of the project. Note, slashes will  al-
// ways  be converted to back slashes when writing paths into the
// project file, regardless of platform.  The  source file, if it
// is  a  relative  path, is interpreted as being relative to CwD
// (not relative to the  project  file  location).  If the source
// file  already  exists in the project file (by case-insensitive
// comparison) then an exception will be thrown. The source  file
// does not need to exist.
void add_2_vcxproj( fs::path const& prj, fs::path const& src ) {

    xml_document doc; xml::parse( doc, prj );

    ASSERT( find_src( doc, prj, src ).empty(),
            "Project file " << prj << " already contains "
            "source file "  << src );

    // See if  there  is  already  an  ItemGroup  containing some
    // ClCompiles. If so then use it,  otherwise we will create a
    // new ItemGroup.
    auto ItemGroup = xml::xpath( "//ItemGroup[ClCompile]", doc );

    // Whether we use an existing one or create a new one, get  a
    // reference to the ItemGroup under which we will add the new
    // source file.
    xml_node parent = ItemGroup.empty()
                    ? xml::node( doc, "/Project" )
                      .append_child( "ItemGroup" )
                    : ItemGroup[0].node();

    // E.g., produces: <ClCompile Include="../../A.cpp" />
    parent.append_child( "ClCompile" )
      .append_attribute( "Include" ) = to_prj( prj, src ).c_str();

    // Rewrite the entire project file, with  the  new  ClCompile.
    ASSERT( doc.save_file( prj.string().c_str(), "  " ),
            "failed to save document to file " << prj );
}

// Open  the  filters  file and insert the given source file into
// it. First we search the filters  file for any other files that
// are  in the same folder as the given one and, if there are, we
// will obtain the name of the filter in that way by choosing the
// first filter that we encounter that contains at least one file
// in the same folder as src. Otherwise, we will  use  the  first
// filter  that  we  find.  The  source file, if it is a relative
// path, is interpreted as being relative to CWD (not relative to
// the project file location). If the source file already  exists
// in the filters file (by case-insensitive comparison)  then  an
// exception will be thrown. The source file does not need to  ex-
// ist.
void add_2_filters( fs::path const& flt, fs::path const& src ) {

    xml_document doc; xml::parse( doc, flt );

    ASSERT( find_src( doc, flt, src ).empty(),
            "Filters file " << flt << " already contains "
            "source file "  << src );

    util::log << "adding " << src << " to " << flt << "\n";

    string filter_name;
    // Now we need to look at the full list of  source  files  in
    // the filters file and find the first one in the same folder
    // as  the new source file (src) and then use its filter name
    // when  adding  the new src file. If we can't find any, then
    // we  will just use the first filter that we find listed. If
    // there  are  no  filters  listed  then  we will use 'Source
    // Files'.
    if( auto ns = find_src_dir( doc, flt, src ); !ns.empty() ) {
        // We  found a src file in same folder, so use its filter.
        auto opt = xml::text( ns[0], "Filter", {}, false, true );
        ASSERT( opt, "missing filter node in " << flt );
        filter_name = *opt;
    } else {
        // Can't find any source  files  in  the  same folder, so
        // just  get  list  of all filter names and use first one.
        util::log << "cannot find any matching source files\n";
        auto xp_flt = "//ItemGroup[Filter]/Filter/@Include";
        auto flts = xml::attrs( doc, xp_flt );
        filter_name = flts.empty() ? "Source Files" : flts[0];
    }

    util::log << "using filter name: " << filter_name << "\n";

    // See if  there  is  already  an  ItemGroup  containing some
    // ClCompiles. If so then use it,  otherwise we will create a
    // new ItemGroup.
    auto ItemGroup = xml::xpath( "//ItemGroup[ClCompile]", doc );

    // Whether we use an existing one or create a new one, get  a
    // reference to the ItemGroup under which we will add the new
    // source file.
    xml_node parent = ItemGroup.empty()
                    ? xml::node( doc, "/Project" )
                      .append_child( "ItemGroup" )
                    : ItemGroup[0].node();

    // <ClCompile Include="..."/><Filter>...</Filter></ClCompile>
    xml_node ClC = parent.append_child( "ClCompile" );
    ClC.append_attribute( "Include" ) = to_prj( flt, src ).c_str();
    ClC.append_child( "Filter" ).text().set( filter_name.c_str() );

    // Rewrite the entire project file, with  the  new  ClCompile.
    ASSERT( doc.save_file( flt.string().c_str(), "  " ),
            "failed to save document to file " << flt );
}

// This  is simply for convenience; it will call add_2_project on
// the given project file,  then  will  construct  a filters file
// path by appending ".filters" to the  end  of the name and will
// then  call  add_2_filters  on  the filters file. If either the
// project file or filters file does  not  exist,  and  exception
// will be thrown. The source file does not need to exist.
void add_2_project( fs::path const& prj,
                    fs::path const& src_file ) {
    add_2_vcxproj( prj, src_file );
    auto filters = prj;
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
// thrown.  Also,  the source file need not exist in the file sys-
// tem; this is to accomodate  cases  where  we  want to remove a
// file from a project file  because  the  user  has  it  locally
// deleted in their working copy.
void rm_from( fs::path const& file,
              fs::path const& src ) {

    xml_document doc; xml::parse( doc, file );

    auto where = find_src( doc, file, src );

    ASSERT( !where.empty(), "Project file " << file << " does "
            "not contain source file " << src );

    // In theory, there could be multiple references to the  same
    // source file, so we just remove  all of them (although this
    // should not happen in practice).
    for( auto n : where )
        n.parent().remove_child( n );

    // Rewrite the entire project file, with  the  new  ClCompile.
    ASSERT( doc.save_file( file.string().c_str(), "  " ),
            "failed to save document to file " << file );
}

// This is simply for convenience; it  will call rm_from with the
// given  project  file,  then will construct the name of the fil-
// ters file by appending .filters to the end of the project file
// and then will call rm_from on that.
void rm_from_project( fs::path const& prj, fs::path const& src ) {

    rm_from( prj, src );
    auto filters = prj;
    filters += ".filters";
    rm_from( filters, src );
}

} // namespace project
