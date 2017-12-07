/****************************************************************
* Project with adjusted file/folder paths
****************************************************************/
#include "normpath.hpp"

using namespace std;

namespace project {

ProjectNorm::ProjectNorm( vector<fs::path>&& cl_includes,
                          vector<fs::path>&& cl_compiles,
                          vector<fs::path>&& search_paths,
                          fs::path&&         int_dir,
                          fs::path&&         out_dir,
                          string&&           project_name,
                          optional<string>&& target_name,
                          optional<string>&& target_ext,
                          string&&           uuid )

  : Project( move( cl_includes  ),
             move( cl_compiles  ),
             move( search_paths ),
             move( int_dir      ),
             move( out_dir      ),
             move( project_name ),
             move( target_name  ),
             move( target_ext   ),
             move( uuid         ) )
{}

auto norm_paths( Project const& p ) -> ProjectNorm {

    auto cl_includes  ( p.cl_includes  );
    auto cl_compiles  ( p.cl_compiles  );
    auto search_paths ( p.search_paths );
    auto int_dir      ( p.int_dir      );
    auto out_dir      ( p.out_dir      );
    auto project_name ( p.project_name );
    auto target_name  ( p.target_name  );
    auto target_ext   ( p.target_ext   );
    auto uuid         ( p.uuid         );

    return ProjectNorm( move( cl_includes  ),
                        move( cl_compiles  ),
                        move( search_paths ),
                        move( int_dir      ),
                        move( out_dir      ),
                        move( project_name ),
                        move( target_name  ),
                        move( target_ext   ),
                        move( uuid         ) );
}

ProjectNorm read_norm( fs::path const& file,
                       string_view     platform ) {
    auto p( read( file, platform ) );
    return norm_paths( p );
}

ostream& operator<<( ostream&           out,
                     ProjectNorm const& p ) {
    return (out << static_cast<Project const&>( p ));
}

} // namespace project
