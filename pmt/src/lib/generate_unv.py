#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

"""
Generate code for uniform numeric vectors
"""

import re, os, os.path


unv_types = (
    ('u8', 'uint8_t'),
    ('s8', 'int8_t'),
    ('u16', 'uint16_t'),
    ('s16', 'int16_t'),
    ('u32', 'uint32_t'),
    ('s32', 'int32_t'),
    ('u64', 'uint64_t'),
    ('s64', 'int64_t'),
    ('f32', 'float'),
    ('f64', 'double'),
    ('c32', 'std::complex<float>'),
    ('c64', 'std::complex<double>')
    )

header = """\
/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
"""

guard_head = """
#ifndef INCLUDED_PMT_UNV_INT_H
#define INCLUDED_PMT_UNV_INT_H
"""

guard_tail = """
#endif
"""

includes = """
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vector>
#include <pmt.h>
#include "pmt_int.h"

"""


# set srcdir to the directory that contains Makefile.am
try:
    srcdir = os.environ['srcdir']
except KeyError, e:
    srcdir = "."
srcdir = srcdir + '/'


def open_src (name, mode):
    global srcdir
    return open(os.path.join (srcdir, name), mode)


def do_substitution (d, input, out_file):
    def repl (match_obj):
        key = match_obj.group (1)
        # print key
        return d[key]
    
    out = re.sub (r"@([a-zA-Z0-9_]+)@", repl, input)
    out_file.write (out)


def generate_h():
    template = open_src('unv_template.h.t', 'r').read()
    output = open('pmt_unv_int.h', 'w')
    output.write(header)
    output.write(guard_head)
    for tag, typ in unv_types:
        d = { 'TAG' : tag, 'TYPE' : typ }
        do_substitution(d, template, output)
    output.write(guard_tail)


def generate_cc():
    template = open_src('unv_template.cc.t', 'r').read()
    output = open('pmt_unv.cc', 'w')
    output.write(header)
    output.write(includes)
    for tag, typ in unv_types:
        d = { 'TAG' : tag, 'TYPE' : typ }
        do_substitution(d, template, output)


def main():
    generate_h()
    generate_cc()

if __name__ == '__main__':
    main()
