/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#include "project.hpp"
#include "project_raw.hpp"
#include "util.hpp"

#include <algorithm>

using namespace std;

namespace project {

Project::Project( ProjectAttr&& pa )
  : attr( move( pa ) )
{}

Project Project::read( fs::path const& file,
                       string_view     platform ) {
    return Project::read( file, "", platform );
}

Project Project::read( fs::path const& file,
                       fs::path const& base,
                       string_view     platform ) {

    auto p = ProjectRaw::read( file, platform );

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

    auto cl_includes  ( p.attr.cl_includes  );
    auto cl_compiles  ( p.attr.cl_compiles  );
    auto search_paths ( p.attr.search_paths );
    auto int_dir      ( p.attr.int_dir      );
    auto out_dir      ( p.attr.out_dir      );
    auto project_name ( p.attr.project_name );
    auto target_name  ( p.attr.target_name  );
    auto target_ext   ( p.attr.target_ext   );
    auto uuid         ( p.attr.uuid         );

    abs_vec( cl_includes  );
    abs_vec( cl_compiles  );
    abs_vec( search_paths );
    int_dir = abs( int_dir );
    out_dir = abs( out_dir );

    return Project{ {
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
    return (out << p.attr);
}

} // namespace project
