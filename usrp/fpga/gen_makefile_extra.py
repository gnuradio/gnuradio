#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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
Generate Makefile.extra
"""

import sys
import os.path

extensions_we_like = (
    '.v', '.vh',
    '.csf', '.esf', '.psf', '.qpf', '.qsf', 
    '.inc', '.cmp', '.bsf',
    '.py')

def visit(keepers, dirname, names):
    if 'rbf' in names:
        names.remove('rbf')
    if 'CVS' in names:
        names.remove('CVS')
    
    if dirname == '.':
        dirname = ''
    if dirname.startswith('./'):
        dirname = dirname[2:]
    
    for n in names:
        base, ext = os.path.splitext(n)
        if ext in extensions_we_like:
            keepers.append(os.path.join(dirname, n))

def generate(f):
    keepers = []
    os.path.walk('.', visit, keepers)
    keepers.sort()
    write_keepers(keepers, f)

def write_keepers(files, outf):
    m = reduce(max, map(len, files), 0)
    e = 'EXTRA_DIST ='
    outf.write('%s%s \\\n' % (e, (m-len(e)+8) * ' '))
    for f in files[:-1]:
        outf.write('\t%s%s \\\n' % (f, (m-len(f)) * ' '))
    outf.write('\t%s\n' % (files[-1],))

if __name__ == '__main__':
    generate(open('Makefile.extra','w'))
