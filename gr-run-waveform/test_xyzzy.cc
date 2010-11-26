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

// Include our definitions
#include "xyzzy.h"

using namespace std;

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

    if (xyzzy.init()) {
        cout << "PASSED: XYZZY::init()" << endl;
    } else {
        cout << "FAILED: XYZZY::init()" << endl;
    }
    
    
// Initialize with the data file produced by gen-xyzzy.
//    xyzzy_init();
// bool xyzzy_init(const std::string &file)
// Does a file with name 'filename' exist in magic filesystem?
//    bool xyzzy_file_exists(handle, const std::string &filespec);
// bool file_exists(const std::string &filespec)

// Return a C port that will read the file contents
//    SCM make_read_only_port(handle, const std::string &filespec)
// SCM xyzzy_make_read_only_port(const std::string &filespec)
    
}
