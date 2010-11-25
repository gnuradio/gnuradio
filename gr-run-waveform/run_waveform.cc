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

using namespace std;

static std::string filename = "hello.scm";

static SCM
load_waveform (void)
{
    ifstream in;
    string filespec = SRCDIR;
    filespec += '/';
    filespec += filename;
    
    in.open(filespec.c_str());
    if (!in) {
        cerr << ("run_waveform: couldn't open data file: ") << filespec << endl;
        return SCM_BOOL_F;
    }

    // Read in the file one line at a time, and accumulate it into
    // a big string that'll hold the entire file.
    string line;
    string file;
    while (std::getline(in, line)) {
        file += line;
        cerr << line << endl;
    }

    // Evaluate the file
    scm_c_eval_string(file.c_str());
    
    in.close();

    return scm_c_eval_string("(hello-world)");
}

static void
inner_main (void *data, int argc, char **argv)
{
    scm_c_define_gsubr ("load-waveform", 0, 0, 0, load_waveform);

    scm_shell (argc, argv);
}

int
main(int argc, char *argv[])
{
    scm_boot_guile (argc, argv, inner_main, 0);

    return 0; // never reached
}
