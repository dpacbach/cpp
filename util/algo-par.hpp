/****************************************************************
* Parallel Algorithms
****************************************************************/
#pragma once

#include "error.hpp"
#include "util.hpp"

#include <algorithm>
#include <functional>
#include <thread>
#include <type_traits>
#include <vector>

namespace util {

// Will  return  the max number of simultaneous threads supported
// on this system. Result will always be >= 1.
int max_threads();

// Will take a vector of functions and will run  each  one  in  a
// separate  thread. The functions are expected to take no parame-
// ters  and  to  return  no values. This is a somewhat low-level
// function that should probably not be called  except  by  other
// functions in this module.
void in_parallel( std::vector<std::function<void()>> const& v );

/* Parallel  map:  apply a function to elements in a range in par-
 * allel.  This  is  being  implemented until the parallel STL be-
 * comes available. Note: the range here expects to have a size()
 * function. Each job processes indexes  from  the input array by
 * starting on the index given (i),  then jumping by intervals of
 * size jobs. */
template<typename FuncT, typename InputT>
auto map_par( FuncT                      func,
              std::vector<InputT> const& input,
              int                        jobs_in = 0 )
{
    // Number of jobs must be valid (which includes zero).
    ASSERT_( jobs_in >= 0 );

    // Interpret zero jobs as a request to use the maximum number
    // of threads available on this system.
    size_t jobs = (jobs_in == 0) ? max_threads() : jobs_in;

    // Create one thread for  each  job,  unless  the size of the
    // input is less than number of  requested jobs. jobs may end
    // up  being  zero  here, and that is ok: at this point, zero
    // jobs means that no threads will  be  spawned  and  nothing
    // will be done (unlike jobs_in == 0 which means to  use  the
    // max number of threads available).
    jobs = std::min( jobs, input.size() );

    // Get  the  underlying value type held by the range and then
    // get the type of result after calling the  function  on  it,
    // stripping away references and const.
    using Payload = std::decay_t<decltype(
        func( *std::begin( input ) )
    )>;

    // The results of calling the function will then be held in a
    // vector of variants, the  variants  being  to  contain  any
    // error  information  if  the  function  call  fails  (a.k.a.
    // throws). Copying should be  disabled  for  the elements of
    // this type, so this should  be  efficiently  moved  to  the
    // caller or, hopefully, NRVO'd.
    std::vector<Result<Payload>> outputs( input.size() );

    // One of the following functions will  be run in each thread.
    auto job = [&]( size_t start ) -> void {

        for( auto i = start; i < input.size(); i += jobs ) {
            try {
                outputs[i] = func( input[i] );
            } catch( std::exception const& e ) {
                outputs[i] = Error{ e.what() };
            } catch( ... ) {
                outputs[i] = Error{ "unknown exception" };
            }
        }
    };

    // Package each job into a void(void) function  that  we  can
    // then hand off to be executed in its own thread.
    std::vector<std::function<void()>> funcs( jobs );
    for( size_t i = 0; i < jobs; ++i )
        // Each job processes indexes from  the  input  array  by
        // starting on the index given (i), then  jumping  by  in-
        // tervals of size jobs.
        funcs[i] = [&job, i](){ return job( i ); };

    // Run the functions in parallel. Each function will run in a
    // single thread and will  handle  a  chunk  of the input ele-
    // ments, storing output in  the  outputs  array which it has
    // captured by reference.
    in_parallel( funcs );

    return outputs;
}

} // namespace util
