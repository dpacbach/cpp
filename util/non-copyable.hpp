/****************************************************************
* Base classes for specifying semantics of derived class
****************************************************************/
#pragma once

namespace util {

// This is a base class used  for  classes  that  should  not  be
// copied but can be move constructed.
struct non_copyable {

    non_copyable()                                 = default;

    non_copyable( non_copyable const& )            = delete;
    non_copyable& operator=( non_copyable const& ) = delete;

    non_copyable( non_copyable&& )                 = default;
    non_copyable& operator=( non_copyable&& )      = default;

};

// This is a base class used  for  classes  that  should  not  be
// moved but can be copy constructed.
struct non_movable {

    non_movable()                                = default;

    non_movable( non_movable const& )            = default;
    non_movable& operator=( non_movable const& ) = default;

    non_movable( non_movable&& )                 = delete;
    non_movable& operator=( non_movable&& )      = delete;

};

// Class for singletons to inherit from:  no  copying  or  moving.
struct singleton : public non_copyable,
                   public non_movable { };

} // namespace util
