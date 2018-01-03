/****************************************************************
* Unit test infrastructure
****************************************************************/
#pragma once

#include "colors.hpp"
#include "macros.hpp"

#include <exception>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace testing {

// These are updated on each  macro  call  and are printed out in
// the  event  that  an exception happens that does not come from
// one of the unit testing macros  in  order  to help the user to
// figure out which line threw the exception.
extern size_t      checkpoint_line;
extern char const* checkpoint_file;

// This  is the type of a unit test function. All that it does is
// to  throw an exception if the test fails. If it returns, it is
// assumed to have passed.
using TestType = void(void);

// Global test list: tests are automatically registered and added
// to this list at static initialization time.
std::vector<TestType*>& test_list();

void run_all_tests();

// Functions for printing to console
std::string fail();
std::string pass();
std::string skip();
std::string bar();

// This  exception is thrown by the unit testing macros to signal
// a failure so that we  can  distinguish  it from other kinds of
// generic std::exception's.
struct failed_exception : public std::exception {
    failed_exception( std::string const& what )
        : msg( what ) {}
    char const* what() const noexcept
        { return msg.c_str(); }
    std::string msg;
};

// This exception is thrown by a  test  to indicate that the test
// should be skipped.
struct skipped_exception : public std::exception {};

} // namespace testing

#define TRUE_( a ) {                                     \
    testing::checkpoint_line = __LINE__;                 \
    testing::checkpoint_file = __FILE__;                 \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << "assert " << #a;                           \
        throw failed_exception( ss.str() );              \
    } }

// a  is an expression that must be true, and b is just something
// printable that will be displayed  in  the  event  a  is  false.
#define TRUE( a, b ) {                                   \
    testing::checkpoint_line = __LINE__;                 \
    testing::checkpoint_file = __FILE__;                 \
    if( !(a) ) {                                         \
        using util::operator<<;                          \
        ostringstream ss;                                \
        ss << "On line " << TO_STRING( __LINE__ ) ": ";  \
        ss << b;                                         \
        throw failed_exception( ss.str() );              \
    } }

#define EQUALS( a, b ) {                                      \
    testing::checkpoint_line = __LINE__;                      \
    testing::checkpoint_file = __FILE__;                      \
    if( !((a) == (b)) ) {                                     \
        using util::operator<<;                               \
        ostringstream ss;                                     \
        ss << "On line " << TO_STRING( __LINE__ ) ": " << #a  \
             << " != " << #b << "\n";                         \
        ss << "instead got: " << (a);                         \
        ss << " ?= " << #b;                                   \
        throw failed_exception( ss.str() );                   \
    } }

#define THROWS( a ) {                                \
    testing::checkpoint_line = __LINE__;             \
    testing::checkpoint_file = __FILE__;             \
    bool STRING_JOIN( __threw_, __LINE__ ) = false;  \
    try {                                            \
        a;                                           \
    } catch( std::exception const& ) {               \
        STRING_JOIN( __threw_, __LINE__ ) = true;    \
    }                                                \
    TRUE( STRING_JOIN( __threw_, __LINE__ ),         \
          "expression: " #a " did not throw." );     \
}

// This is used to create a unit test function.
// TODO: move this code into a cpp function to avoid  duplicating
// it in test.cpp and to reduce compile times.
#define TEST( a )                                           \
    void STRING_JOIN( __test_, a )();                       \
    void STRING_JOIN( test_, a )() {                        \
        using util::operator<<;                             \
        testing::checkpoint_line = __LINE__;                \
        testing::checkpoint_file = __FILE__;                \
        string test = string( "Test " ) + TO_STRING( a );   \
        cout << left << setw( 40 ) << test;                 \
        enum class Res { PASSED, SKIPPED, FAILED };         \
        Res result = Res::FAILED;                           \
        string err;                                         \
        try {                                               \
            STRING_JOIN( __test_, a )();                    \
            result = Res::PASSED;                           \
        } catch( testing::skipped_exception const& ) {      \
            result = Res::SKIPPED;                          \
        } catch( testing::failed_exception const& e ) {     \
            err = e.what();                                 \
            result = Res::FAILED;                           \
        } catch( std::exception const& e ) {                \
            err = e.what();                                 \
            err += "\nLast checkpoint: ";                   \
            err += std::string( checkpoint_file );          \
            err += ", line ";                               \
            err += util::to_string( checkpoint_line );      \
            result = Res::FAILED;                           \
        } catch( ... ) {                                    \
            err = "unknown exception";                      \
            result = Res::FAILED;                           \
        }                                                   \
        cerr << "     | ";                                  \
        bool failed = false;                                \
        switch( result ) {                                  \
        case Res::FAILED:                                   \
            failed = true;                                  \
            cerr << testing::fail() << "\n";                \
            cerr << testing::bar() << "\n";                 \
            cerr << err << "\n";                            \
            cerr << testing::bar() << "\n";                 \
            break;                                          \
        case Res::SKIPPED:                                  \
            cout << testing::skip();                        \
            break;                                          \
        case Res::PASSED:                                   \
            cout << testing::pass();                        \
            break;                                          \
        }                                                   \
        cout << util::c_norm << "\n";                       \
        if( failed ) throw runtime_error( "test failed" );  \
    }                                                       \
    STARTUP() {                                             \
        test_list().push_back( STRING_JOIN( test_, a ) );   \
    }                                                       \
    void STRING_JOIN( __test_, a )()
