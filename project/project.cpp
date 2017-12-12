/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#include "project.hpp"
#include "parser.hpp"
#include "string-util.hpp"
#include "util.hpp"

#include <algorithm>

using namespace std;

namespace project {

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
