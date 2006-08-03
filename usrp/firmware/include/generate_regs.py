#!/usr/bin/env python

import os, os.path
import re
import sys


# set srcdir to the directory that contains Makefile.am
try:
    srcdir = os.environ['srcdir']
except KeyError, e:
    srcdir = "."
srcdir = srcdir + '/'

def open_src (name, mode):
    global srcdir
    return open (os.path.join (srcdir, name), mode)


def generate_fpga_regs (h_filename, v_filename):
    const_width = 7                  # bit width of constants
    
    h_file = open_src (h_filename, 'r')
    v_file = open (v_filename, 'w')
    v_file.write (
    '''//
// This file is machine generated from %s
// Do not edit by hand; your edits will be overwritten.
//
''' % (h_filename,))

    pat = re.compile (r'^#define\s*(FR_\w*)\s*(\w*)(.*)$')
    pat_bitno = re.compile (r'^#define\s*(bitno\w*)\s*(\w*)(.*)$')
    pat_bm = re.compile (r'^#define\s*(bm\w*)\s*(\w*)(.*)$')
    for line in h_file:
        if re.match ('//|\s*$', line):  # comment or blank line
            v_file.write (line)
        mo = pat.search (line)
        mo_bitno =pat_bitno.search (line)
        mo_bm =pat_bm.search (line)
        if mo:
            v_file.write ('`define %-25s %d\'d%s%s\n' % (
                mo.group (1), const_width, mo.group (2), mo.group (3)))
        elif mo_bitno:
            v_file.write ('`define %-25s %s%s\n' % (
                mo_bitno.group (1), mo_bitno.group (2), mo_bitno.group (3)))
        elif mo_bm:
            v_file.write ('`define %-25s %s%s\n' % (
                mo_bm.group (1), mo_bm.group (2), mo_bm.group (3)))
    

if __name__ == '__main__':
    if len (sys.argv) != 3:
        sys.stderr.write ('usage: %s file.h file.v\n' % (sys.argv[0]))
        sys.exit (1)
    generate_fpga_regs (sys.argv[1], sys.argv[2])
    
