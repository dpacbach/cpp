/****************************************************************
* Non-copyable bass class
****************************************************************/
#pragma once

namespace util {

// This is a base class used  for  classes  that  should  not  be
// copying but can be move constructable.
struct non_copyable {

    non_copyable()                                 = default;

    non_copyable( non_copyable const& )            = delete;
    non_copyable& operator=( non_copyable const& ) = delete;

    non_copyable( non_copyable&& )                 = default;
    non_copyable& operator=( non_copyable&& )      = default;

};

}
