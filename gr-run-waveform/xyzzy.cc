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
        cerr << ("run_waveform: couldn't open data file: ") << file << endl;
        return SCM_BOOL_F;
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

    cout << "There are " << header->number_of_dir_entries << " of directory entries" << endl;
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
    cout << "Loaded " << _contents.size() << " Filesystem entries" << endl;
    
    in.close();
    
    return true;
};

// Does a file with name 'filename' exist in magic filesystem?
//    bool file_exists(handle, const std::string &filespec);
bool
XYZZY::file_exists(const std::string &filespec)
{
    std::map<std::string, std::string>::iterator it;
    it = _contents.find(filespec);
    if (it != _contents.end()) {
        return true;
    }
    return false;
}

// Return a C port that will read the file contents
//    SCM make_read_only_port(handle, const std::string &filespec)
SCM
XYZZY::make_read_only_port(const std::string &filespec)
{
    scm_t_bits bits = scm_make_port_type("gnuradio", xyzzy_fill_input, xyzzy_write);
    scm_set_port_flush (bits, xyzzy_flush);
    scm_set_port_close (bits, xyzzy_close);
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

void
xyzzy_write (SCM port, const void *data, size_t size)
{
    // This is a read only file
}

void
xyzzy_flush (SCM port)
{
    SCM bport = SCM_PACK(SCM_STREAM(port));
    scm_t_port *c_port = SCM_PTAB_ENTRY (port);
    size_t count = c_port->write_pos - c_port->write_buf;
    
    scm_c_write (bport, c_port->write_buf, count);
    
    c_port->write_pos = c_port->write_buf;
    c_port->rw_active = SCM_PORT_NEITHER;
    
    scm_force_output (bport);
}

int
xyzzy_close (SCM port)
{
    if (SCM_OUTPUT_PORT_P (port)) {
        // XYZZY::flush (port);
        return scm_is_true (scm_close_port (SCM_PACK(SCM_STREAM(port)))) ? 0 : -1;
    }
    return 0;
}
    
int
xyzzy_fill_input (SCM port)
{
    scm_t_port *gr_port = SCM_PTAB_ENTRY (port);
    if (gr_port->read_pos + gr_port->read_buf_size > gr_port->read_end) {
        return EOF;
    }

    // if (_contents.empty()) {
    //     gr_port->read_buf; // = datafile.file_exists("FOO");
    //     gr_port->read_end = gr_port->read_buf + gr_port->read_buf_size;
    //     gr_port->read_pos += gr_port->read_buf_size;
    // }
    
    return *gr_port->read_buf;
}

// Initialize with the data file produced by gen-xyzzy.
bool
xyzzy_init(const std::string &filespec)
{
    return datafile.init(filespec);
}

// Does a file with name 'filename' exist in magic filesystem?
bool
xyzzy_file_exists(const std::string &filespec)
{
    return datafile.file_exists(filespec);
}

// Return a C port that will read the file contents
SCM
xyzzy_make_read_only_port(const std::string &filespec)
{
    return datafile.make_read_only_port(filespec);
}
    
} // end of extern C
