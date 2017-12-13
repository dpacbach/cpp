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
    return {};
}

auto lib_name( ProjectAttr const& attr ) -> OptPath {
    return {};
}

auto pdb_name( ProjectAttr const& attr ) -> OptPath {
    return {};
}

auto exp_name( ProjectAttr const& attr ) -> OptPath {
    return {};
}

auto src_folders( ProjectAttr const& attr ) -> PathVec {
    return {};
}

ostream& operator<<( ostream& out, ProjectAttr const& attr ) {

    auto p = [&]( auto const& p ) {
        out << "    | " << p << endl;
    };

    auto p_ = [&]( auto const& s ) {
        print( util::to_string( s ) );
    };

    auto print_v = [&]( auto const& v ) {
        for( auto const& s : v ) p( s );
    };

    out << "Search Paths : " << endl; p_v( attr.search_paths );
    out << "ClCompile    : " << endl; p_v( attr.cl_compiles );
    out << "ClInclude    : " << endl; p_v( attr.cl_includes );
    out << "IntDir       : " << endl; p  ( attr.int_dir );
    out << "OutDir       : " << endl; p  ( attr.out_dir );
    out << "ProjectName  : " << endl; p_ ( attr.project_name );
    out << "TargetName   : " << endl; p_ ( attr.target_name );
    out << "TargetExt    : " << endl; p_ ( attr.target_ext );
    out << "UUID         : " << endl; p_ ( attr.uuid );
    out << "tlog name    : " << endl; p  ( tlog_name(  attr ) );

    return out;
}

} // namespace project
