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
        f.write("    ")
        if cw & mask:
            f.write("h->%s = ntohl(p[%d]);\n" % (name, index))
            return 1
        else:
            f.write("h->%s = 0;\n" % (name,))
            return 0
        
    def do64(name, mask, index):
        f.write("    ")
        if cw & mask:
            f.write("h->%s = ((uint64_t)(ntohl(p[%d])) << 32) | ntohl(p[%d]);\n" % (name, index, index+1))
            return 2
        else:
            f.write("h->%s = 0;\n" % (name,))
            return 0

    def dotrailer(name, mask):
        f.write("    ")
        if cw & mask:
            f.write("h->%s = ntohl(p[len-1]);\n" % (name,))
            return 1
        else:
            f.write("h->%s = 0;\n" % (name,))
            return 0
        
    f.write("  case %d:\n" % (cw,))

    index = 1
    index += do32("stream_id", HAS_STREAM_ID, index)
    index += do64("class_id",  HAS_CLASS_ID,  index)
    index += do32("integer_secs", HAS_INTEGER_SECS, index)
    index += do64("fractional_secs", HAS_FRACTIONAL_SECS, index)
    dotrailer("trailer", HAS_TRAILER)
    
    f.write("    break;\n\n")
        

def main():
    f = sys.stdout

    for cw in range(32):
        do_case(f, cw)


if __name__ == '__main__':
    main()
