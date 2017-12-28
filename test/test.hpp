/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "colors.hpp"
#include "macros.hpp"

#include <iomanip>
#include <string>

namespace testing {

std::string failed() {
    return util::c_red + "Failed" + util::c_norm;
}

std::string passed() {
    return util::c_green + "Passed" + util::c_norm;
}

} // namespace testing

#define TRUE( a )                                                       \
    if( !(a) ) {                                                        \
        cerr << "--------------------------------------------" << endl; \
        cerr << testing::failed() << ": assert " << #a << endl;         \
        cerr << "--------------------------------------------" << endl; \
        throw logic_error( "test failed." );                            \
    }

#define EQUALS( a, b )                                                  \
    if( !((a) == (b)) ) {                                               \
        cerr << endl;                                                   \
        cerr << "--------------------------------------------" << endl; \
        cerr << testing::failed() << " on line "                        \
             << TO_STRING( __LINE__ ) ": " << #a                        \
             << " != " << #b << endl;                                   \
        cerr << "instead got: " << (a);                                 \
        cerr << " ?= " << #b << endl;                                   \
        cerr << "--------------------------------------------" << endl; \
        throw logic_error( "test failed." );                            \
    }

// This is used to create a unit test function.
#define TEST( a )                                         \
    void STRING_JOIN( __test_, a )();                     \
    void STRING_JOIN( test_, a )() {                      \
        string test = string( "Test " ) + TO_STRING( a ); \
        cout << left << setw( 40 ) << test;               \
        STRING_JOIN( __test_, a )();                      \
        cout << " ==> " << testing::passed();             \
        cout << util::c_norm << endl;                     \
    }                                                     \
    void STRING_JOIN( __test_, a )()
