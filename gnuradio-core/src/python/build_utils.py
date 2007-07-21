#
# Copyright 2004 Free Software Foundation, Inc.
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

"""Misc utilities used at build time
"""

import re, os, os.path
from build_utils_codes import *


# set srcdir to the directory that contains Makefile.am
try:
    srcdir = os.environ['srcdir']
except KeyError, e:
    srcdir = "."
srcdir = srcdir + '/'


name_dict = {}

def log_output_name (name):
    (base, ext) = os.path.splitext (name)
    ext = ext[1:]                       # drop the leading '.'
    
    entry = name_dict.setdefault (ext, [])
    entry.append (name)
    
def open_and_log_name (name, dir):
    f = open (name, dir)
    log_output_name (name)
    return f

def expand_template (d, template_filename, extra = ""):
    '''Given a dictionary D and a TEMPLATE_FILENAME, expand template into output file
    '''
    output_extension = extract_extension (template_filename)
    template = open_src (template_filename, 'r')
    output_name = d['NAME'] + extra + '.' + output_extension
    log_output_name (output_name)
    output = open (output_name, 'w')
    do_substitution (d, template, output)
    template.close ()
    output.close ()

def output_glue (dirname):
    output_makefile_fragment ()
    output_ifile_include (dirname)
    
def output_makefile_fragment ():
    f = open ('Makefile.gen', 'w')
    f.write ('#\n# This file is machine generated.  All edits will be overwritten\n#\n')
    output_subfrag (f, 'h')
    output_subfrag (f, 'i')
    output_subfrag (f, 'cc')
    f.close ()

def output_ifile_include (dirname):
    f = open ('%s_generated.i' % (dirname,), 'w')
    f.write ('//\n// This file is machine generated.  All edits will be overwritten\n//\n')
    files = name_dict.setdefault ('i', [])
    files.sort ()
    f.write ('%{\n')
    for file in files:
        f.write ('#include <%s>\n' % (file[0:-1] + 'h',))
    f.write ('%}\n\n')
    for file in files:
        f.write ('%%include <%s>\n' % (file,))

def output_subfrag (f, ext):
    files = name_dict.setdefault (ext, [])
    files.sort ()
    f.write ("GENERATED_%s =" % (ext.upper ()))
    for file in files:
        f.write (" \\\n\t%s" % (file,))
    f.write ("\n\n")
    

def extract_extension (template_name):
    # template name is something like: GrFIRfilterXXX.h.t
    # we return everything between the penultimate . and .t
    mo = re.search (r'\.([a-z]+)\.t$', template_name)
    if not mo:
        raise ValueError, "Incorrectly formed template_name '%s'" % (template_name,)
    return mo.group (1)

def open_src (name, mode):
    global srcdir
    return open (os.path.join (srcdir, name), mode)

def do_substitution (d, in_file, out_file):
    def repl (match_obj):
        key = match_obj.group (1)
        # print key
        return d[key]
    
    inp = in_file.read ()
    out = re.sub (r"@([a-zA-Z0-9_]+)@", repl, inp)
    out_file.write (out)

    

copyright = '''/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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
'''

def is_complex (code3):
    if i_code (code3) == 'c' or o_code (code3) == 'c':
        return '1'
    else:
        return '0'


def standard_dict (name, code3):
    d = {}
    d['NAME'] = name
    d['GUARD_NAME'] = 'INCLUDED_%s_H' % name.upper ()
    d['BASE_NAME'] = re.sub ('^gr_', '', name)
    d['SPTR_NAME'] = '%s_sptr' % name
    d['WARNING'] = 'WARNING: this file is machine generated.  Edits will be over written'
    d['COPYRIGHT'] = copyright
    d['TYPE'] = i_type (code3)
    d['I_TYPE'] = i_type (code3)
    d['O_TYPE'] = o_type (code3)
    d['TAP_TYPE'] = tap_type (code3)
    d['IS_COMPLEX'] = is_complex (code3)
    return d
