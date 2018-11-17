/****************************************************************
* Unit tests
****************************************************************/
#include "common-test.hpp"

#include "io.hpp"
#include "md5-util.hpp"
#include "string-util.hpp"

using namespace std;

fs::path const data_common = "../test/data-common";
fs::path const data_local  = "../test/data-local";

namespace testing {

TEST( md5 )
{
    vector<char> v;
    EQUALS( crypto::md5( v ).size(), 32 );
    EQUALS(
        crypto::md5(
            v
        ), "d41d8cd98f00b204e9800998ecf8427e"
    );

    vector<char> v2{ 'a', 'b', 'c' };
    EQUALS( crypto::md5( v2 ).size(), 32 );
    EQUALS(
        crypto::md5(
            v2
        ), "900150983cd24fb0d6963f7d28e17f72"
    );

    EQUALS(
        crypto::md5(
           ""
        ), "d41d8cd98f00b204e9800998ecf8427e"
    );

    EQUALS(
        crypto::md5(
           "a"
        ), "0cc175b9c0f1b6a831c399e269772661"
    );

    EQUALS(
        crypto::md5(
           "abc"
        ), "900150983cd24fb0d6963f7d28e17f72"
    );

    EQUALS(
        crypto::md5(
           "message digest"
        ), "f96b697d7cb7938d525a2f31aaf161d0"
    );

    EQUALS(
        crypto::md5(
           "abcdefghijklmnopqrstuvwxyz"
        ), "c3fcd3d76192e4007dfb496cca67e13b"
    );

    EQUALS(
        crypto::md5(
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
           "fghijklmnopqrstuvwxyz0123456789"
        ), "d174ab98d277d9f5a5611c2c9f419d9f"
    );

    EQUALS(
        crypto::md5(
           "1234567890123456789012345678901234567890"
           "1234567890123456789012345678901234567890"
        ), "57edf4a22be3c955ac49da2e2107b67a"
    );

    EQUALS(
        crypto::md5(
           "This string is precisely 56 characters "
           "long for a reason"
        ), "93d268e9bef6608ff1a6a96adbeee106"
    );

    EQUALS(
        crypto::md5(
           "This string is exactly 64 characters long "
           "for a very good reason"
        ), "655c37c2c8451a60306d09f2971e49ff"
    );

    EQUALS(
        crypto::md5(
           "This string is also a specific length.  "
           "It is exactly 128 characters long for a "
           "very good reason as well. We are testing "
           "bounds."
        ), "2ac62baa5be7fa36587c55691c026b35"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaa"
        ), "e09c80c42fda55f9d992e59ca6b3307d"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        ), "014842d480b571495a4a0363793f7367"
    );

    EQUALS(
        crypto::md5(
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
           "aaaaaaaaa"
        ), "b64e4f62e3e14317e3a90f9ff2cde576"
    );

    // Test computing the checksum of  random  binary  data  read
    // from a file.
    vector<char> bin = util::read_file( data_common / "random.bin" );
    EQUALS( crypto::md5( bin ), "e4cf202b4e919fc8c68ca2753fc8d737" );
}

} // namespace testing
