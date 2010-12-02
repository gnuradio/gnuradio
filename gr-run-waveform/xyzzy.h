/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef _XYZZY_H_
#define _XYZZY_H_ 1


#include <libguile.h>

#ifdef __cplusplus

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

using namespace std;

// - Special case filenames that start with /-xyzzy-/... and search for
//   and load them using the single file.  We'd stick "/-zyzzy-" first
//   in the default load-path.

// - Create a C read-only "port" that "reads" from the string in the file.
//   (See guile docs on creating new kinds of ports)

// - Override the default implementation of "primitive-load" and "%search-load-path"
//   to make that happen.  See load.c in the guile source code.  Figure
//   out how to get the override done before guile is fully
//   initialized.  (Guile loads ice-9/boot9.scm to start itself up.  We'd
//   need to redirect before then.)

struct header {
    char     magic[8];
    
    boost::uint32_t offset_to_directory;	// byte offset from start of file
    boost::uint32_t size_of_directory;         // bytes
    boost::uint32_t number_of_dir_entries;
    
    boost::uint32_t offset_to_strings;	// byte offset from start of file
    boost::uint32_t size_of_strings;	// bytes
};

struct directory_entry {
    boost::uint32_t offset_to_name;	 // from start of strings
    boost::uint32_t offset_to_contents;  // from start of strings
};
    
// Each string starts with a uint32_t length, followed by length bytes.
// There is no trailing \0 in the string.    
struct string_entry {
    boost::uint32_t length;
    boost::uint8_t  *base;
};

class XYZZY {
public:
    XYZZY();
    ~XYZZY();

    // Initialize with the data file produced by gen-xyzzy.
    bool init();
    bool init(const std::string &filespec);
    
    // Does a file with name 'filename' exist in magic filesystem?
    bool file_exists(const std::string &filespec);

    /// Parse a string data structure
    static std::string read_string(boost::uint8_t *entry, size_t length);
    static std::string read_string(struct string_entry &entry);
    static std::string read_string(std::ifstream &stream);

    // Read the header of the datafile
    boost::shared_ptr<struct header> read_header(boost::uint8_t *header);
    
    boost::shared_ptr<struct directory_entry> read_dir_entry(boost::uint8_t *header);

    std::string &get_contents(const std::string &filespec) { return _contents[filespec]; };
    
private:
    std::string   _filespec;
    std::map<std::string, std::string> _contents;
};

// C linkage bindings for Guile
extern "C" {
#endif

void scm_xyzzy_init (void);
    
// Initialize with the data file produced by gen-xyzzy.
int xyzzy_init(const char *filename);

// Does a file with name 'filename' exist in magic filesystem?
int xyzzy_file_exists(const char *filename);

// Make a readonly port
SCM make_xyzzy (SCM binary_port);
    
#ifdef __cplusplus
} // end of extern C
#endif

#endif  // _XYZZY_H_ 1
