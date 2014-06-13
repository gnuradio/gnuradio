#!/usr/bin/env python
#
# Copyright 2006,2009 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
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
 * Copyright 2006,2009 Free Software Foundation, Inc.
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
"""

guard_tail = """
#endif
"""

includes = """
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vector>
#include <pmt/pmt.h>
#include <boost/lexical_cast.hpp>
#include "pmt_int.h"
"""

qa_includes = """
#include <qa_pmt_unv.h>
#include <cppunit/TestAssert.h>
#include <pmt/pmt.h>
#include <stdio.h>

using namespace pmt;
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


def guard_name(filename):
    return 'INCLUDED_' + re.sub('\.', '_', filename.upper())

def guard_head(filename):
    guard = guard_name(filename)
    return """
#ifndef %s
#define %s
""" % (guard, guard)


def do_substitution (d, input, out_file):
    def repl (match_obj):
        key = match_obj.group (1)
        # print key
        return d[key]

    out = re.sub (r"@([a-zA-Z0-9_]+)@", repl, input)
    out_file.write (out)


def generate_h():
    template = open_src('unv_template.h.t', 'r').read()
    output_filename = 'pmt_unv_int.h'
    output = open(output_filename, 'w')
    output.write(header)
    output.write(guard_head(output_filename))
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


def generate_qa_h():
    output_filename = 'qa_pmt_unv.h'
    output = open(output_filename, 'w')
    output.write(header)
    output.write(guard_head(output_filename))

    output.write('''
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_pmt_unv : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_pmt_unv);
''')
    for tag, typ in unv_types:
        output.write('  CPPUNIT_TEST(test_%svector);\n' % (tag,))
    output.write('''\
  CPPUNIT_TEST_SUITE_END();

 private:
''')
    for tag, typ in unv_types:
        output.write('  void test_%svector();\n' % (tag,))
    output.write('};\n')
    output.write(guard_tail)

def generate_qa_cc():
    template = open_src('unv_qa_template.cc.t', 'r').read()
    output = open('qa_pmt_unv.cc', 'w')
    output.write(header)
    output.write(qa_includes)
    for tag, typ in unv_types:
        d = { 'TAG' : tag, 'TYPE' : typ }
        do_substitution(d, template, output)


def main():
    generate_h()
    generate_cc()
    generate_qa_h()
    generate_qa_cc()

if __name__ == '__main__':
    main()
