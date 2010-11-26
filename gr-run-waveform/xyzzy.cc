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

// Include our definitions
#include "xyzzy.h"

using namespace std;

typedef void* handle_t;

handle_t datafile;


// Initialize with the data file produced by gen-xyzzy.
bool init()
{
    string filespec = PKGLIBDIR;
    filespec += '/';
    filespec += "filesystem.dat";
    return init(filespec);
};

bool init(const std::string &file)
{
    return false;
};

// Does a file with name 'filename' exist in magic filesystem?
//    bool file_exists(handle, const std::string &filespec);
bool
file_exists(const std::string &filespec)
{
//    datafile
}

// Return a C port that will read the file contents
//    SCM make_read_only_port(handle, const std::string &filespec)
SCM
make_read_only_port(const std::string &filespec)
{
//    datafile
}
