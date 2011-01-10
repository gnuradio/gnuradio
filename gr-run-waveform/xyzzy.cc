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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <libguile.h>
#include <libguile/ports.h>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

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
    _contents.clear();
}

// Initialize with the data file produced by gen-xyzzy.
bool
XYZZY::init()
{
    _filespec = DATAROOTDIR;
    _filespec += "/gnuradio/gr-run-waveform/";
    _filespec += "filesystem.dat";
    return init(_filespec);
};

bool
XYZZY::init(const std::string &file)
{
    ifstream in(file.c_str(), ios_base::binary|ios_base::in);
    if (!in) {
        cerr << ("gr-run-waveform: couldn't open data file: ") << file << endl;
        return false;
    }

    size_t length = sizeof(struct header);
    char head[length];
    in.read(head, length);

    boost::shared_ptr<struct header> header = read_header(reinterpret_cast<boost::uint8_t *>(&head));

    // Check the magic number to make sure it's the right type of file.
    if (strncmp(header->magic, "-XyZzY-", 8) != 0) {
        cerr << "ERROR: bad magic number" << endl;
        return false;
    }

    // Read in the Directory table
    length = sizeof(struct directory_entry);
    char dir[length];    
    boost::uint32_t ssize;
    for (size_t i=0; i<header->number_of_dir_entries; ++i) {
        in.read(dir, length);
        int store = in.tellg();
        boost::shared_ptr<struct directory_entry> entry = read_dir_entry(
            reinterpret_cast<boost::uint8_t *>(dir));

        // Get the file name
        in.seekg(header->offset_to_strings + entry->offset_to_name);
        string name = XYZZY::read_string(in);
        // cout << name << endl;
        
        // Get the contents, which is one big string
        in.seekg(header->offset_to_strings + entry->offset_to_contents);
        string contents = XYZZY::read_string(in);
        // cout << contents << endl;
        in.seekg(store);
        _contents[name] = contents;
    }
    // cout << "Loaded " << _contents.size() << " Filesystem entries" << endl;
    
    in.close();

    return true;
};

// Does a file with name 'filename' exist in magic filesystem?
//    bool file_exists(handle, const std::string &filespec);
bool
XYZZY::file_exists(const std::string &filespec)
{
    if (filespec.substr(0, 9) == "/-xyzzy-/") {
        // look for prefix
        std::map<std::string, std::string>::iterator it;
        it = _contents.find(filespec.substr(9, filespec.size()));
        if (it != _contents.end()) {
            return true;
        }
        return false;
    }
    return false;
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

string
XYZZY::read_string(std::ifstream &stream)
{
    boost::uint32_t length;
    char num[sizeof(boost::uint32_t)];
    
    stream.read(reinterpret_cast<char *>(&length), sizeof(boost::uint32_t));
    boost::uint32_t len = __builtin_bswap32(length);
    // All the strings are 32 bit word aligned, so we have to adjust
    // how many bytes to read.
    size_t padding = sizeof(boost::uint32_t) - (len % sizeof(boost::uint32_t));
    size_t newsize = (padding == 4) ? len : len + padding;
    char sstr[newsize];
    
    // Read the string
    stream.read(sstr, newsize);
    
    string filespec(reinterpret_cast<const char *>(sstr), len);
    return filespec;
}

boost::shared_ptr<struct header>
XYZZY::read_header(boost::uint8_t *header)
{
    boost::shared_ptr<struct header> newhead(new struct header);
    
    struct header *ptr = reinterpret_cast<struct header *>(header);

    std::copy(header, header + 8, newhead->magic);

    newhead->offset_to_directory   = __builtin_bswap32(ptr->offset_to_directory);
    newhead->size_of_directory     = __builtin_bswap32(ptr->size_of_directory);
    newhead->number_of_dir_entries = __builtin_bswap32(ptr->number_of_dir_entries);
    newhead->offset_to_strings     = __builtin_bswap32(ptr->offset_to_strings);
    newhead->size_of_strings       = __builtin_bswap32(ptr->size_of_strings);
    
    return newhead;
}

boost::shared_ptr<struct directory_entry>
XYZZY::read_dir_entry(boost::uint8_t *entry)
{
    boost::shared_ptr<struct directory_entry> newdir(new struct directory_entry);
    struct directory_entry *ptr = reinterpret_cast<struct directory_entry *>(entry);

    newdir->offset_to_name     = __builtin_bswap32(ptr->offset_to_name);
    newdir->offset_to_contents = __builtin_bswap32(ptr->offset_to_contents);

    return newdir;
}

// C linkage for guile
extern "C" {

static XYZZY datafile;
    
SCM
xyzzy_open_file(SCM filename)
#define FUNC_NAME "xyzzy-open-file"
{
  const char *c_filename = scm_to_locale_string(filename);
  // fprintf(stderr, "TRACE %s: %d, %s\n", __FUNCTION__, __LINE__, c_filename);

  if (!xyzzy_file_exists(c_filename)){
    SCM_MISC_ERROR("file does not exist: ~S", scm_list_1(filename));
  }

  std::string filespec(c_filename);
  std::string &contents = datafile.get_contents(filespec.substr(9, filespec.size()));
  SCM port = scm_open_input_string (scm_from_locale_string (contents.c_str()));
    
  return port;
}
#undef FUNC_NAME

// Initialize with the data file produced by gen-xyzzy.
int
xyzzy_init(const char *filespec)
{
  if (filespec == 0 || *filespec == 0)
    return datafile.init();
  else
    return datafile.init(filespec);
}

// Does a file with name 'filename' exist in magic filesystem?
int
xyzzy_file_exists(const char *filespec)
{
    return datafile.file_exists(filespec);
}
    
} // end of extern C
