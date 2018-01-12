/****************************************************************
* SVN-status Utilities
****************************************************************/
#include "status-util.hpp"
#include "util.hpp"

#include <map>

using namespace std;

namespace svn {

namespace {

namespace xpaths {

   /* Typical entries looks like:
    *
    *     <entry path="file_name.ext">
    *         <wc-status props="none" item="unversioned">
    *         </wc-status>
    *     </entry>
    *     <entry path="another_file.abc">
    *         <wc-status props="none" item="modified">
    *         </wc-status>
    *     </entry>
    */

    char const* entries = "//entry";

    char const* path    = "@path";

    char const* item    = "wc-status/@item";

    char const* tc      = "wc-status/@tree-conflicted";

}

} // anonymous namespace

#define CHANGE( a ) { #a, Status::ChangeType::a }

// Global map from  enum  to  corresponding string representation.
CTMap g_change{

    CHANGE( modified    ),
    CHANGE( added       ),
    CHANGE( deleted     ),
    CHANGE( conflicted  ),
    CHANGE( unversioned )

};

// Take a string holding  the  output  of  the `svn status --xml`
// command and parse it,  then  extract  enough information to re-
// turn a list Status descriptors. Throws  on  error  or  if  the
// structure of the xml is not exactly as expected.
auto parse_status_xml( string xml ) -> vector<Status> {

    pugi::xml_document doc;

    xml::parse( doc, xml );

    vector<Status> res;

    for( auto entry : xml::xpath( xpaths::entries, doc ) ) {

        ASSERT( entry.node(), "`entry` node in SVN status xml "
               "is not a node!" );
        auto const& n = entry.node();

        Status s;

        auto item = xml::attr( n, xpaths::item, {}, false );
        ASSERT( item, "failed to find precisely one non-empty "
                      "`item` attribute on a wc-status subnode "
                      "of the `entry` node in the SVN status "
                      "XML output. There may be 0, or more "
                      "than one. " );

        // If  it's  not  a key that we're concerned with then we
        // won't even include this item in the result.
        if( !g_change.val_safe( *item ) )
            continue;

        s.change = g_change.val( *item );

        auto p = xml::attr( n, xpaths::path, {}, false );
        ASSERT( p, "failed to find precisely one non-empty "
                   "`path` attribute on the entry nodes in the "
                   "SVN status XML output. There may be 0, or "
                   "more than one. " );
        s.path = util::fwd_slashes( *p );

        auto tc = xml::attr( n, xpaths::tc, {}, true );
        // If the wc-status node has an attribute called
        // "tree-conflicted"  and  if the value of this attribute
        // is "true" then it is a tree conflict.
        s.tree_conflict = (tc && *tc == "true");

        res.push_back( move( s ) );
    }

    // Not sure why we need to do this, but there may have been a
    // good reason, so do it since it won't hurt.
    util::uniq_sort( res );
    return res;
}

// For convenience.
ostream& operator<<( ostream&      out,
                     Status const& status ) {
    return (out << util::to_string( status ));
}

ostream& operator<<( ostream& out,
                     Status::ChangeType const& ct ) {
    return (out << util::to_string( ct ));
}

// These operators are to allow us to do the unique sort  on  the
// list of Status objects.

bool operator<( Status const& lhs, Status const& rhs ) {
    return std::tie( lhs.path, lhs.change, lhs.tree_conflict ) <
           std::tie( rhs.path, rhs.change, rhs.tree_conflict );
}

bool operator==( Status const& lhs, Status const& rhs ) {
    return std::tie( lhs.path, lhs.change, lhs.tree_conflict ) ==
           std::tie( rhs.path, rhs.change, rhs.tree_conflict );
}

} // namespace svn

namespace util {

template<>
string to_string( svn::Status const& s ) {
    return string( "(" ) +
        "path="   + to_string( s.path          ) + ", " +
        "change=" + to_string( s.change        ) + ", " +
        "tc="     + to_string( s.tree_conflict ) +
    ")";
}

template<>
string to_string( svn::Status::ChangeType const& s ) {
    return string( "\"" ) + svn::g_change.key( s ) + "\"";
}

} // namespace util
