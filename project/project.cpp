/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#include "project.hpp"
#include "parser.hpp"
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

    auto cl_includes  ( p.cl_includes  );
    auto cl_compiles  ( p.cl_compiles  );
    auto search_paths ( p.search_paths );
    auto int_dir      ( p.int_dir      );
    auto out_dir      ( p.out_dir      );
    auto project_name ( p.project_name );
    auto target_name  ( p.target_name  );
    auto target_ext   ( p.target_ext   );
    auto uuid         ( p.uuid         );

    abs_vec( cl_includes  );
    abs_vec( cl_compiles  );
    abs_vec( search_paths );
    int_dir = abs( int_dir );
    out_dir = abs( out_dir );

    return Project{ {
        {},   // ProjectAttr base
        move( cl_includes  ),
        move( cl_compiles  ),
        move( search_paths ),
        move( int_dir      ),
        move( out_dir      ),
        move( project_name ),
        move( target_name  ),
        move( target_ext   ),
        move( uuid         )
    } };
}

ostream& operator<<( ostream& out, Project const& p ) {
    return (out << p.attr());
}

} // namespace project
