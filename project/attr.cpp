/****************************************************************
* Project Attributes
****************************************************************/
#include "attr.hpp"
#include "opt-util.hpp"
#include "util.hpp"

#include <iostream>

using namespace std;

namespace project {

auto tlog_name( ProjectAttr const& attr ) -> fs::path {
    string res;
    if( attr.project_name.size() <= 16 )
        res = attr.project_name;
    else
        res = attr.project_name.substr( 0, 8 ) + "." +
              attr.uuid.substr( 0, 8 );
    return res + ".tlog";
}

auto target_name( ProjectAttr const& attr ) -> OptPath {
    fs::path res = attr.target_stem.value_or( "" )
                 + attr.target_ext .value_or( "" );
    return res.empty() ? nullopt : OptPath( move( res ) );
}

auto lib_name( ProjectAttr const& attr ) -> OptPath {
    return attr.target_stem ? OptPath( *attr.target_stem+".lib" )
                            : nullopt;
}

auto pdb_name( ProjectAttr const& attr ) -> OptPath {
    return attr.target_stem ? OptPath( *attr.target_stem+".pdb" )
                            : nullopt;
}

auto exp_name( ProjectAttr const& attr ) -> OptPath {
    return attr.target_stem ? OptPath( *attr.target_stem+".ext" )
                            : nullopt;
}

// Get a list of all compiled  files  in project, get the folders
// in  which  they reside, and return a unique list of them (i.e.,
// most projects should  return  just  a  small  handful of items
// here).
auto src_folders( ProjectAttr const& attr ) -> PathVec {
    PathVec v = attr.cl_compiles;
    auto parent = []( fs::path& p ){ return p.parent_path(); };
    transform( begin( v ), end( v ), begin( v ), parent );
    sort( begin( v ), end( v ) );
    auto new_end = unique( begin( v ), end( v ) );
    v.erase( new_end, end( v ) );
    return v;
}

ostream& operator<<( ostream& out, ProjectAttr const& attr ) {

    auto p = [&]( auto const& p ) {
        out << p << endl;
    };

    auto ps = [&]( auto const& s ) {
        out << util::to_string( s ) << endl;
    };

    // Logic in this function is just for some nice
    // alignment on the console.
    auto p_v = [&]( auto const& v ) {
        if( v.empty() ) {
            out << endl;
            return;
        }
        bool first = true;
        for( auto const& s : v ) {
            if( first ) {
                p( s );
                first = false;
                continue;
            }
            out << "             | ";
            p( s );
        }
    };

    out << "Search Paths | "; p_v( attr.search_paths );
    out << "ClCompile    | "; p_v( attr.cl_compiles );
    out << "ClInclude    | "; p_v( attr.cl_includes );
    out << "Src Folders  | "; p_v( src_folders( attr ) );
    out << "IntDir       | "; p  ( attr.int_dir );
    out << "OutDir       | "; p  ( attr.out_dir );
    out << "ProjectName  | "; ps ( attr.project_name );
    out << "TargetName   | "; ps ( attr.target_stem );
    out << "TargetExt    | "; ps ( attr.target_ext );
    out << "UUID         | "; ps ( attr.uuid );
    out << "tlog name    | "; p  ( tlog_name(  attr ) );
    out << "Target Name  | "; ps ( target_name( attr ) );
    out << "Lib Name     | "; ps ( lib_name( attr ) );
    out << "PDB Name     | "; ps ( pdb_name( attr ) );
    out << "exp Name     | "; ps ( exp_name( attr ) );

    return out;
}

} // namespace project
