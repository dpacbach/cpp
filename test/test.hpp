/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "macros.hpp"

#define TRUE( a )                                                       \
    if( !(a) ) {                                                        \
        cerr << "--------------------------------------------" << endl; \
        cerr << "FAILED: assert " << #a << endl;                        \
        cerr << "--------------------------------------------" << endl; \
        throw logic_error( "test failed." );                            \
    }

#define EQUALS( a, b )                                                  \
    auto STRING_JOIN( a_, __LINE__ ) = a;                               \
    auto STRING_JOIN( b_, __LINE__ ) = b;                               \
    if( STRING_JOIN( a_, __LINE__ ) != STRING_JOIN( b_, __LINE__) ) {   \
        cerr << "--------------------------------------------" << endl; \
        cerr << "FAILED: " << #a << " != " << #b << endl;               \
        cerr << "instead got: " << STRING_JOIN( a_, __LINE__ );         \
        cerr << " ?= " << #b << endl;                                   \
        cerr << "--------------------------------------------" << endl; \
        throw logic_error( "test failed." );                            \
    }

// This is used to create a unit test function.
#define TEST( a )                                       \
    void STRING_JOIN( __test_, a )();                   \
    void STRING_JOIN( test_, a )() {                    \
        cout << "Tests for " << TO_STRING( a ) << endl; \
        STRING_JOIN( __test_, a )();                    \
        cout << "          ==> Passed." << endl;        \
    }                                                   \
    void STRING_JOIN( __test_, a )()
