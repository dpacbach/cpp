/****************************************************************
* Project Attributes
****************************************************************/
#include "attr.hpp"
#include "opt-util.hpp"
#include "util.hpp"

#include <iostream>

using namespace std;

namespace project {

auto tlog_name( ProjectAttr const& attr ) -> string {
    string res;
    if( attr.project_name.size() <= 16 )
        res = attr.project_name;
    else
        res = attr.project_name.substr( 0, 8 ) + "." +
              attr.uuid.substr( 0, 8 );
    return res + ".tlog";
}

ostream& operator<<( ostream& out, ProjectAttr const& attr ) {

    auto print = [&]( auto const& s ) {
            out << "  | \"" << util::to_string( s )
                << "\"" << endl;
    };

    auto print_path = [&]( auto const& p ) {
            out << "  | " << p << endl;
    };

    auto print_path_list = [&]( auto const& v ) {
        for( auto const& s : v )
            print_path( s );
    };

    out << "AdditionaIncludeDirectories: " << endl;
    print_path_list( attr.search_paths );
    out << "ClCompile: " << endl;
    print_path_list( attr.cl_compiles );
    out << "ClInclude: " << endl;
    print_path_list( attr.cl_includes );
    out << "IntDir: " << endl;
    print_path( attr.int_dir );
    out << "OutDir: " << endl;
    print_path( attr.out_dir );
    out << "ProjectName: " << endl;
    print( attr.project_name );
    out << "TargetName: " << endl;
    print( attr.target_name );
    out << "TargetExt: " << endl;
    print( attr.target_ext );
    out << "UUID: " << endl;
    print( attr.uuid );
    out << "tlog name: " << endl;
    print( tlog_name( attr ) );

    return out;
}

} // namespace project
