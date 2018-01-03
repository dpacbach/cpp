/****************************************************************
* Mock Preprocessor
****************************************************************/
#include "preprocessor.hpp"

#include "algo-par.hpp"
#include "logger.hpp"
#include "macros.hpp"
#include "stopwatch.hpp"
#include "string-util.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <thread>

using namespace std;

namespace project {

namespace {

// This  is a regex to scan for a #include preprocessor statement.
// Note: it allows mismatching quotes  or brackets, i.e., the fol-
// lowing is valid: #include  "a/b/c>.  Oh  well. Also, hopefully
// this will be compiled once (as  a  global variable) so that it
// is faster to use multiple times.
regex re_inc( "^[ \t]*#[ \t]*include[ \t]*[\"<]([^\"> ]*)[\">].*" );

// These are a list of extensions marking files that are relevant
// to this preprocessor.
vector<fs::path> const exts{
    ".cpp", ".hpp", ".c", ".h", ".inl", ".cu", ".cuh", ".cc"
};

// Returns true if p has extension in the above list (with
// case-insensitive comparison).
bool is_interesting( fs::path const& p ) {

    auto ext = p.extension();
    util::CaseSensitive sens = util::CaseSensitive::NO;

    auto pred = [&]( fs::path const& a ) {
        return util::path_equals( ext, a, sens );
    };

    return find_if( begin( exts ), end( exts ), pred )
           != end( exts );
}

}

// This method will, in a quite flexible way, scan for an include
// statement in the string and, if a  valid one is found, it will
// return the contents of the quotes or brackets.
OptPath parse_include( string const& sv ) {
    smatch m;
    if( !regex_match( sv, m, re_inc ) )
        return nullopt;
    ASSERT_( m.size() == 2 );
    return string( m[1] );
}

// Open a file and parse every line for an include statement  and
// return a list of the inner contents (file/path).
PathVec parse_includes( fs::path const& file ) {

    ifstream in( file.string() );
    ASSERT( in.good(), "failed to open file " << file );

    PathVec res;

    for( string line; getline( in, line ); ) {
        // Strip the line to eliminate any windows  line  endings
        // which getline may leave  (though  it should remove the
        // linux line endings.
        line = util::strip( line );
        auto path = parse_include( line );
        if( !path )
            continue;
        res.push_back( *path );
    }
    return res; // hoping for NRVO here
}

// Find all source files under the `from` paths, open them up and
// parse all the include statements  in  them,  then return a map
// where key is the file path relative to base_path and the value
// is a list of parsed (but not resolved) include files.
GlobalIncludeMap build_sources( PathVec const& from,
                                fs::path       base_path ) {
    ASSERT_( base_path.is_absolute() );
    GlobalIncludeMap res;
    for( auto const& folder : from ) {
        ASSERT( folder.is_absolute(),
                folder << " is not an absolute path." );
        for( auto& i : fs::recursive_directory_iterator( folder ) ) {
            if( is_interesting( i ) ) {
                auto rel = util::lexically_relative( i, base_path );
                // Use i here since we don't want to assume that  the
                // CWD is equal to base_path.
                res[rel] = parse_includes( i );
            }
        }
    }
    return res;
}

// Take  a  relative  path and a list of search paths and resolve
// the path using (approximately) the algorithm that the compiler
// would  use, which means to iterate through the search paths in
// order, construct a candidate path  from  each,  and then to to
// stop as soon as a match is  found, where a match is defined as
// a file that is found  in  the  global  file map. Note that, as
// some compilers do, we first  search  in the current_path which
// is supposed to represent the  folder  containing the file that
// included this file under examination.  Note  that the paths in-
// volved are expected to all be in normal form.
//
// When a match is  found  it  will  be  returned  as a reference
// (wrapper) to the path stored inside the global mapping,  other-
// wise nullopt.
OptCRef<fs::path> resolve( GlobalIncludeMap const& m,
                           fs::path         const& current,
                           PathVec          const& search_paths,
                           fs::path         const& relative ) {

    auto abs = [&relative]( auto const& p ) {
        // The relative path may have double dots in it.
        return util::lexically_normal( p / relative );
    };

    if( auto ref = util::get_key_safe( m, abs( current ) ); ref )
        return ref;

    for( auto const& s : search_paths )
        if( auto ref = util::get_key_safe( m, abs( s ) ); ref )
            return ref;

    return nullopt;
}

// Runs  the  `resolve`  function  over  a list of relative paths.
// paths that do not resolve are simply ignored.
PathCRefVec resolves( GlobalIncludeMap const& m,
                      fs::path         const& current,
                      PathVec          const& search_paths,
                      PathVec          const& relatives ) {
    // Not all relative paths may  resolve, but usually they will,
    // so it is worth reserving that much memory.
    PathCRefVec res; res.reserve( relatives.size() );
    for( auto const& r : relatives )
        if( auto o = resolve( m, current, search_paths, r ); o )
            res.push_back( *o );

    return res;
}

// Same as above but looks up the `current path` and list  of  in-
// cluded relative paths from the global map.
PathCRefVec resolves( GlobalIncludeMap const& m,
                      PathVec          const& search_paths,
                      fs::path         const& file ) {
    auto current = file.parent_path();
    // Throws if file not in map.
    auto const& relatives = util::get_val( m, file );
    return resolves( m, current, search_paths, relatives );
}

// This will essentially run the preprocessor on  a  project  and
// produce the final directed graph. Note that the directed graph
// will  contain  only  files encountered starting from the speci-
// fied sources, and all references  to  files will be references
// to  within  the  global  map. Note that sources are also refer-
// ences to within the global map.
util::DirectedGraph<PathCRef>
preprocess( GlobalIncludeMap const& m,
            PathVec          const& search_paths,
            PathCRefVec      const& sources ) {

    GlobalRefIncludeMap mres;

    // Initialize stack with all source files.
    PathCRefVec st = sources;

    while( !st.empty() ) {
        PathCRef p = st.back(); st.pop_back();
        if( util::has_key( mres, p ) )
            continue;
        mres[p] = resolves( m, search_paths, p.get() );
        for( auto p : mres[p] )
            if( !util::has_key( mres, p ) )
                st.push_back( p );
    }
    return util::make_graph<PathCRef>( mres );
}

// Same as above, but takes a project, and writes out the results
// to the Cl.read.1.tlog file in the  intermediate  folder.  Will
// return the file name that it wrote to.
fs::path preprocess_project( GlobalIncludeMap const& m,
                             fs::path         const& base,
                             Project          const& project ) {
    auto const& attr = project.attr();
    PathCRefVec sources; sources.reserve( attr.cl_compiles.size() );
    for( auto const& p : attr.cl_compiles ) {
        auto o = util::get_key_safe( m, p );
        ASSERT( o, "Source file " << p << " not found in global map." );
        sources.push_back( *o );
    }
    auto graph = preprocess( m, attr.search_paths, sources );

    auto cl_read = util::lexically_normal(
            base / attr.int_dir / "CL.read.1.tlog" );
    // Here we use create_directory  which  will create the inter-
    // mediate  folder  if  it  does  not exist (and not throw an
    // error if it already does)  but  will not create the parent
    // folders; those are assumed to  exist  and  it should be an
    // error if they don't, so we want it to throw in  that  case.
    fs::create_directory( cl_read.parent_path() );
    ofstream out( cl_read.string() );
    ASSERT( out.good(), "failed to open " << cl_read );
    for( auto s : sources ) {
        // Have to call the string() method so that it will
        // output without quotes.
        out << "^" << s.get().string() << "\n";
        // `false` means don't include s in results.
        for( auto n : graph.accessible( s, false ) )
            out << n.get().string() << "\n";
    }
    return cl_read;
}

// Will  open  the  solution,  parse  it, parse all project files
// therein, and then call preprocess on each project.
void preprocess_solution( GlobalIncludeMap const& m,
                          fs::path         const& base,
                          Solution         const& solution,
                          int                     jobs ) {

    auto const& ps = solution.projects();

    vector<fs::path> items; items.reserve( ps.size() );
    for( auto const& p : ps )
        items.push_back( p.first );

    // Process all projects in parallel.
    util::par::for_each( items, [&]( fs::path const& p ) {

        auto cl_read = preprocess_project( m, base, ps.at( p ) );
        util::log << "wrote to " << cl_read.string() << "\n";

    }, jobs );
}

// Will  open  the  solution,  parse  it, parse all project files
// therein, and then call preprocess on each project.
void preprocess_solution( GlobalIncludeMap const& m,
                          fs::path         const& base,
                          fs::path         const& solution,
                          string_view             platform,
                          int                     jobs ) {
    auto s = TIMEIT( "read full solution",
        Solution::read( solution, platform, base )
    );
    TIMEIT( "preprocess full solution",
        preprocess_solution( m, base, s, jobs )
    );
}

// This will run the whole preprocessor.
void run_preprocessor( fs::path const& base_folder,
                       PathVec  const& source_folders,
                       fs::path const& solution_file,
                       StrVec   const& platforms,
                       int             jobs ) {

    auto global = TIMEIT( "build global map",
        build_sources( source_folders, base_folder );
    );

    for( auto const& p : platforms ) {
        util::log << "running preprocessor for " << p << "\n";
        preprocess_solution( global,
                             base_folder,
                             solution_file,
                             p,
                             jobs );
    }
}

} // namespace project
