#!/usr/bin/env python
#
# Copyright 2004,2006 Free Software Foundation, Inc.
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

import re
import sys
import os, os.path
from optparse import OptionParser

# USB Vendor and Product ID's

VID = 0xfffe                            # Free Software Folks
PID = 0x0002                            # Universal Software Radio Peripheral


def hex_to_bytes (s):
    if len (s) & 0x1:
        raise ValueError, "Length must be even"
    r = []
    for i in range (0, len(s), 2):
        r.append (int (s[i:i+2], 16))
    return r
    
def msb (x):
    return (x >> 8) & 0xff

def lsb (x):
    return x & 0xff

class ihx_rec (object):
    def __init__ (self, addr, type, data):
        self.addr = addr
        self.type = type
        self.data = data

class ihx_file (object):
    def __init__ (self):
        self.pat = re.compile (r':[0-9A-F]{10,}')
    def read (self, file):
        r = []
        for line in file:
            line = line.strip().upper ()
            if not self.pat.match (line):
                raise ValueError, "Invalid hex record format"
            bytes = hex_to_bytes (line[1:])
            sum = reduce (lambda x, y: x + y, bytes, 0) % 256
            if sum != 0:
                raise ValueError, "Bad hex checksum"
            lenx = bytes[0]
            addr = (bytes[1] << 8) + bytes[2]
            type = bytes[3]
            data = bytes[4:-1]
            if lenx != len (data):
                raise ValueError, "Invalid hex record (bad length)"
            if type != 0:
                break;
            r.append (ihx_rec (addr, type, data))

        return r

def get_code (filename):
    """Read the intel hex format file FILENAME and return a tuple
    of the code starting address and a list of bytes to load there.
    """
    f = open (filename, 'r')
    ifx = ihx_file ()
    r = ifx.read (f)
    r.sort (lambda a,b: a.addr - b.addr)
    code_start = r[0].addr
    code_end = r[-1].addr + len (r[-1].data)
    code_len = code_end - code_start
    code = [0] * code_len
    for x in r:
        a = x.addr
        l = len (x.data)
        code[a-code_start:a-code_start+l] = x.data
    return (code_start, code)
        

def build_eeprom_image (filename, rev):
    """Build a ``C2 Load'' EEPROM image.

    For details on this format, see section 3.4.3 of
    the EZ-USB FX2 Technical Reference Manual
    """
    # get the code we want to run
    (start_addr, bytes) = get_code (filename)

    devid = rev

    rom_header = [
        0xC2,                           # boot from EEPROM
        lsb (VID),
        msb (VID),
        lsb (PID),
        msb (PID),
        lsb (devid),
        msb (devid),
        0                               # configuration byte
        ]
    
    # 4 byte header that indicates where to load
    # the immediately follow code bytes.
    code_header = [
        msb (len (bytes)),
        lsb (len (bytes)),
        msb (start_addr),
        lsb (start_addr)
        ]

    # writes 0 to CPUCS reg (brings FX2 out of reset)
    trailer = [
        0x80,
        0x01,
        0xe6,
        0x00,
        0x00
        ]

    image = rom_header + code_header + bytes + trailer

    assert (len (image) <= 256)
    return image

def build_shell_script (out, ihx_filename, rev, prefix):

    image = build_eeprom_image (ihx_filename, rev)

    out.write ('#!/bin/sh\n')
    out.write ('usrper -x load_firmware ' + prefix + '/share/usrp/rev%d/std.ihx\n' % rev)
    out.write ('sleep 2\n')
    
    # print "len(image) =", len(image)
    
    i2c_addr = 0x50
    rom_addr = 0x00

    hex_image = map (lambda x : "%02x" % (x,), image)

    while (len (hex_image) > 0):
        l = min (len (hex_image), 16)
        out.write ('usrper i2c_write 0x%02x %02x%s\n' %
                   (i2c_addr, rom_addr, ''.join (hex_image[0:l])))
        hex_image = hex_image[l:]
        rom_addr = rom_addr + l
        out.write ('sleep 2\n')

if __name__ == '__main__':
    usage = "usage: %prog -p PREFIX -r REV [options] bootfile.ihx"
    parser = OptionParser (usage=usage)
    parser.add_option ("-p", "--prefix", type="string", default="",
                       help="Specify install prefix from configure")
    parser.add_option ("-r", "--rev", type="int", default=-1,
                       help="Specify USRP revision number REV (2 or 4)")
    (options, args) = parser.parse_args ()
    if len (args) != 1:
        parser.print_help ()
        sys.exit (1)
    if options.rev < 0:
        sys.stderr.write (
            "You must specify the USRP revision number (2 or 4) with -r REV\n")
        sys.exit (1)
    if options.prefix == "":
        sys.stderr.write (
            "You must specify the install prefix with -p PREFIX\n")
        sys.exit (1)

    ihx_filename = args[0]

    build_shell_script (sys.stdout, ihx_filename, options.rev, options.prefix)
