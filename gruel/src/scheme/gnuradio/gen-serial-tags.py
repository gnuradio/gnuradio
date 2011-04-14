"""
//
// Copyright 2011 Free Software Foundation, Inc.
//
// This file is part of GNU Radio
//
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//
// THIS FILE IS MACHINE GENERATED FROM %s. DO NOT EDIT BY HAND.
// See %s for additional commentary.
//

#ifndef INCLUDED_PMT_SERIAL_TAGS_H
#define INCLUDED_PMT_SERIAL_TAGS_H

enum pst_tags {
%s
};
#endif /* INCLUDED_PMT_SERIAL_TAGS_H */
"""

import sys, os, re

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage %s <input_scm_file> <output_hdr_file>"%__file__
        exit()
    input_scm_file, output_hdr_file = sys.argv[1:]
    enums = list()
    for line in open(input_scm_file).readlines():
        match = re.match('^\s*\(define\s+([\w|-]+)\s+#x([0-9a-fA-F]+)\)', line)
        if not match: continue
        name, value = match.groups()
        name = name.upper().replace('-', '_')
        enums.append('    %s = 0x%s'%(name, value))
    open(output_hdr_file, 'w').write(__doc__%(
        os.path.basename(__file__),
        os.path.basename(input_scm_file),
        ',\n'.join(enums),
    ))
