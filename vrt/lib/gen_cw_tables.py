#!/usr/bin/env python
#
# Copyright 2009 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

import sys

# dispatch codeword bits
HAS_STREAM_ID       = 1 << 0;
HAS_CLASS_ID        = 1 << 1;
HAS_INTEGER_SECS    = 1 << 2;
HAS_FRACTIONAL_SECS = 1 << 3;
HAS_TRAILER         = 1 << 4;

def main():
    f = sys.stdout
    header_len  = 32 * [0]
    trailer_len = 32 * [0]

    for cw in range(32):
        t = 0
        if cw & HAS_TRAILER:         t += 1
        trailer_len[cw] = t

        t = 1
        if cw & HAS_STREAM_ID:       t += 1
        if cw & HAS_CLASS_ID:        t += 2
        if cw & HAS_INTEGER_SECS:    t += 1
        if cw & HAS_FRACTIONAL_SECS: t += 2
        header_len[cw] = t

    write_table(f, "cw_header_len", header_len)
    write_table(f, "cw_trailer_len", trailer_len)
    
def write_table(f, name, table):
    f.write("inline static size_t ")
    f.write(name)
    f.write("(int cw){\n")

    f.write("  static const size_t s_")
    f.write(name)
    f.write("[32] = {\n    ")
    for t in table:
        f.write("%d, " % (t,))
    f.write("\n  };\n")

    f.write("  return s_")
    f.write(name)
    f.write("[cw];\n}\n\n")


if __name__ == '__main__':
    main()
