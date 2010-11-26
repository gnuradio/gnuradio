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

typedef void* handle_t;

XYZZY::XYZZY()
{
    // nothing to initialize
}

XYZZY::~XYZZY()
{
    // nothing to destruct
}

// Initialize with the data file produced by gen-xyzzy.
bool
XYZZY::init()
{
    _filespec = PKGLIBDIR;
    _filespec += '/';
    _filespec += "filesystem.dat";
    return init(_filespec);
};

bool
XYZZY::init(const std::string &file)
{
    return false;
};

// Does a file with name 'filename' exist in magic filesystem?
//    bool file_exists(handle, const std::string &filespec);
bool
XYZZY::file_exists(const std::string &filespec)
{

}

// Return a C port that will read the file contents
//    SCM make_read_only_port(handle, const std::string &filespec)
SCM
XYZZY::make_read_only_port(const std::string &filespec)
{

}

string
XYZZY::read_string(boost::uint8_t *entry, size_t length)
{
    string str(reinterpret_cast<const char *>(entry), length);
    
    return str;
}

string
XYZZY::read_string(struct string_entry & entry)
{
    return read_string(entry.base, entry.length);
}

extern "C" {

static XYZZY datafile;
    
// Initialize with the data file produced by gen-xyzzy.
bool xyzzy_init(const std::string &filespec)
{
    datafile.init(filespec);
}

// Does a file with name 'filename' exist in magic filesystem?
bool xyzzy_file_exists(const std::string &filespec)
{
    datafile.file_exists(filespec);
}

// Return a C port that will read the file contents
SCM xyzzy_make_read_only_port(const std::string &filespec)
{
    datafile.make_read_only_port(filespec);
}

} // end of extern C
