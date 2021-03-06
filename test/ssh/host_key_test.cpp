/**
    @file

    Tests for host_key object.

    @if license

    Copyright (C) 2010, 2013  Alexander Lamaison <awl03@doc.ic.ac.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    In addition, as a special exception, the the copyright holders give you
    permission to combine this program with free software programs or the 
    OpenSSL project's "OpenSSL" library (or with modified versions of it, 
    with unchanged license). You may copy and distribute such a system 
    following the terms of the GNU GPL for this program and the licenses 
    of the other code concerned. The GNU General Public License gives 
    permission to release a modified version without this exception; this 
    exception also makes it possible to release a modified version which 
    carries forward this exception.

    @endif
*/

#include "session_fixture.hpp" // session_fixture

#include <ssh/detail/session_state.hpp>
#include <ssh/host_key.hpp> // test subject
#include <ssh/session.hpp> // session

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

#include <libssh2.h>

using ssh::detail::session_state;
using ssh::host_key;
using ssh::session;

using test::ssh::session_fixture;

using boost::make_shared;
using boost::shared_ptr;
using boost::system::error_code;
using boost::system::system_error;

using std::string;

const string EXPECTED_HOSTKEY =
    "AAAAB3NzaC1yc2EAAAABIwAAAQEArrr/JuJmaZligyfS8vcNur+mWR2ddDQtVdhHzdKU"
    "UoR6/Om6cvxpe61H1YZO1xCpLUBXmkki4HoNtYOpPB2W4V+8U4BDeVBD5crypEOE1+7B"
    "Am99fnEDxYIOZq2/jTP0yQmzCpWYS3COyFmkOL7sfX1wQMeW5zQT2WKcxC6FSWbhDqrB"
    "eNEGi687hJJoJ7YXgY/IdiYW5NcOuqRSWljjGS3dAJsHHWk4nJbhjEDXbPaeduMAwQU9"
    "i6ELfP3r+q6wdu0P4jWaoo3De1aYxnToV/ldXykpipON4NPamsb6Ph2qlJQKypq7J4iQ"
    "gkIIbCU1A31+4ExvcIVoxLQw/aTSbw==";

BOOST_FIXTURE_TEST_SUITE(host_key_tests, session_fixture)

namespace {

    string base64_decode(const string& input)
    {
        shared_ptr<session_state> session = make_shared<session_state>();

        char* data;
        unsigned int data_len;
        int rc = libssh2_base64_decode(
            session->session_ptr(), &data, &data_len, input.data(),
            input.size());
        if (rc)
        {
            string message;
            error_code ec = ssh::detail::last_error_code(
                session->session_ptr(), message);

            BOOST_THROW_EXCEPTION(system_error(ec, message));
        }

        string out(data, data_len);
        free(data);
        return out;
    }
}

/**
 * Server hostkey corresponds to test key when connected.
 */
BOOST_AUTO_TEST_CASE( hostkey )
{
    session& s = test_session();
    host_key key = s.hostkey();

    string expected = base64_decode(EXPECTED_HOSTKEY);
    BOOST_CHECK_EQUAL(key.key(), expected);
    BOOST_CHECK_EQUAL(key.algorithm(), ssh::hostkey_type::ssh_rsa);
    BOOST_CHECK_EQUAL(key.algorithm_name(), "ssh-rsa");
    BOOST_CHECK(!key.is_base64());
}

/**
 * Hashed (MD5) hostkey should print as:
 *    0C 0E D1 A5 BB 10 27 5F 76 92 4C E1 87 CE 5C 5E
 * in hex.
 */
BOOST_AUTO_TEST_CASE( hostkey_md5 )
{
    host_key key = test_session().hostkey();

    string hex_hash(ssh::hexify(key.md5_hash(), " ", true));

    BOOST_CHECK_EQUAL(
        hex_hash, "0C 0E D1 A5 BB 10 27 5F 76 92 4C E1 87 CE 5C 5E");
}

BOOST_AUTO_TEST_SUITE_END();
