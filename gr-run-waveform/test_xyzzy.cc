/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <libguile.h>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

// Include our definitions
#include "xyzzy.h"

using namespace std;

boost::uint8_t hex2digit(boost::uint8_t digit);
boost::shared_array<boost::uint8_t> hex2mem(const std::string &str);

static SCM
test_xyzzy (void)
{
    cerr << "FIXME:" << endl;
    SCM port;
    scm_input_port_p(port);
}

static void
inner_main (void *data, int argc, char **argv)
{
    scm_c_define_gsubr ("test-xyzzy", 0, 0, 0, test_xyzzy);

    scm_shell (argc, argv);
}

int
main(int argc, char *argv[])
{
    // Test static methods
    string hello("Hello World!");
    boost::uint8_t *hi = reinterpret_cast<boost::uint8_t *>(const_cast<char *>
                                                            (hello.c_str()));
    struct string_entry entry = {
        12,
        hi
    };
    
    if (XYZZY::read_string(entry) == hello) {
        cout << "PASSED: XYZZY::read_string(struct string_entry &)" << endl;
    } else {
        cout << "FAILED: XYZZY::read_string(struct string_entry &)" << endl;
    }
    
    if (XYZZY::read_string(hi, hello.size()) == hello) {
        cout << "PASSED: XYZZY::read_string(struct string_entry &)" << endl;
    } else {
        cout << "FAILED: XYZZY::read_string(struct string_entry &)" << endl;
    }

    // Test other methods
    XYZZY xyzzy;

    string fake_magic("-XyZzY-");
    boost::shared_array<boost::uint8_t> fake_header_data = hex2mem(
        "2d 58 79 5a 7a 59 2d 00 00 00 00 1c 00 00 05 e8 00 00 00 bd 00 00 06 04 00 21 ee 58");
    boost::shared_ptr<struct header> head = xyzzy.read_header(fake_header_data.get());
    if ((head->magic == fake_magic)
        && (head->offset_to_directory == 28)
        && (head->size_of_directory == 1512)
        && (head->number_of_dir_entries == 189)
        && (head->offset_to_strings == 1540)
        && (head->size_of_strings == 2223704)) {
        cout << "PASSED: XYZZY::read_header()" << endl;
    } else {
        cout << "FAILED: XYZZY::read_header()" << endl;
    }

#if 0
    if (xyzzy.init()) {
        cout << "PASSED: XYZZY::init()" << endl;
    } else {
        cout << "FAILED: XYZZY::init()" << endl;
    }
#endif

    // Look for a file to exist, which shouldn't as we haven't loaded the data
    if (xyzzy_file_exists("srfi/srfi-35.scm")) {
        cout << "FAILED: xyzzy_file_exists(not yet)" << endl;
    } else {
        cout << "PASSED: xyzzy_file_exists(not yet)" << endl;
    }    

    // Initialize with the data file produced by gen-xyzzy.
    string fullspec = "./filesystem.dat";
    char *name = const_cast<char *>(fullspec.c_str());
    if (xyzzy_init(name)) {
        cout << "PASSED: xyzzy_init()" << endl;
    } else {
        cout << "FAILED: xyzzy_init()" << endl;
    }
     
    // Does a file with name 'filename' exist in magic filesystem?
    if (xyzzy_file_exists("srfi/srfi-35.scm")) {
        cout << "FAILED: xyzzy_file_exists(shouldn't exist)" << endl;
    } else {
        cout << "PASSED: xyzzy_file_exists(shouldn't exist)" << endl;
    }
    
    // Does a file with name 'filename' exist in magic filesystem?
    if (xyzzy_file_exists("/-xyzzy-/srfi/srfi-35.scm")) {
        cout << "PASSED: xyzzy_file_exists(should exist)" << endl;
    } else {
        cout << "FAILED: xyzzy_file_exists(should exist)" << endl;
    }
    
    // Does a file with name 'filename' exist in magic filesystem?
    if (xyzzy_file_exists("/-xyzzy-/srfi/srfi-99.scm")) {
        cout << "FAILED: xyzzy_file_exists(shouldn't exist)" << endl;
    } else {
        cout << "PASSED: xyzzy_file_exists(shouldn't exist)" << endl;
    }
    
    scm_boot_guile (argc, argv, inner_main, 0);   
}


/// \brief Convert a Hex digit into it's decimal value.
///
/// @param digit The digit as a hex value
///
/// @return The byte as a decimal value.
boost::uint8_t
hex2digit (boost::uint8_t digit)
{  
    if (digit == 0)
        return 0;
    
    if (digit >= '0' && digit <= '9')
        return digit - '0';
    if (digit >= 'a' && digit <= 'f')
        return digit - 'a' + 10;
    if (digit >= 'A' && digit <= 'F')
        return digit - 'A' + 10;
    
    // shouldn't ever get this far
    return -1;
}

/// \brief Encode a Buffer from a hex string.
///
/// @param str A hex string, ex... "00 03 05 0a"
///
/// @return A reference to a Buffer in host endian format. This is
///		primary used only for testing to create binary data
///		from an easy to read and edit format.
boost::shared_array<boost::uint8_t>
hex2mem(const std::string &str)
{
//    GNASH_REPORT_FUNCTION;
    size_t count = str.size();
    
    size_t size = (count/3) + 4;
    boost::uint8_t ch = 0;
    
    boost::shared_array<boost::uint8_t> data(new boost::uint8_t[count]);

    size_t j = 0;
    for (size_t i=0; i<count; i++) {
        if (str[i] == ' ') {      // skip spaces.
            continue;
        }
        ch = hex2digit(str[i]) << 4;
        ch |= hex2digit(str[i+1]);
        data[j++] = ch;
        i++;
    }
    
    return data;
}
