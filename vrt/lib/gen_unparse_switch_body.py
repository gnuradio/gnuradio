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

def do_case(f, cw):

    def do32(name, mask, index):
        if cw & mask:
            f.write("    header[%d] = htonl(h->%s);\n" % (index, name))
            return 1
        return 0
        
    def do64(name, mask, index):
        if cw & mask:
            f.write("    header[%d] = htonl((uint32_t)((h->%s >> 32) & 0xffffffff));\n" % (index, name))
            f.write("    header[%d] = htonl((uint32_t)((h->%s >>  0) & 0xffffffff));\n" % (index+1, name))
            return 2
        return 0

    def dolength(index):
        f.write("    *n32_bit_words_header = %d;\n"%index)

    def dotrailer(name, mask):
        if cw & mask:
            f.write("    trailer[%d] = htonl(h->%s);\n" % (0, name))
            f.write("    *n32_bit_words_trailer = 1;\n")
            return 1
        else:
            f.write("    *n32_bit_words_trailer = 0;\n")
            return 0
        
    f.write("  case %d:\n" % (cw,))

    index = 1
    index += do32("stream_id", HAS_STREAM_ID, index)
    index += do64("class_id",  HAS_CLASS_ID,  index)
    index += do32("integer_secs", HAS_INTEGER_SECS, index)
    index += do64("fractional_secs", HAS_FRACTIONAL_SECS, index)
    dolength(index)
    dotrailer("trailer", HAS_TRAILER)
    
    f.write("    break;\n\n")
        

def main():
    f = sys.stdout

    for cw in range(32):
        do_case(f, cw)


if __name__ == '__main__':
    main()
