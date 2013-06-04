#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

import sys
from gnuradio import gr, blocks
import pmt

'''
sr    Sample rate (samples/second)
time  Time as uint64(secs), double(fractional secs)
type  Type of data (see gr_file_types enum)
cplx  is complex? (True or False)
strt  Start of data (or size of header) in bytes
size  Size of data in bytes
'''

HEADER_LENGTH = blocks.METADATA_HEADER_SIZE

ftype_to_string = {blocks.GR_FILE_BYTE: "bytes",
                   blocks.GR_FILE_SHORT: "short",
                   blocks.GR_FILE_INT: "int",
                   blocks.GR_FILE_LONG: "long",
                   blocks.GR_FILE_LONG_LONG: "long long",
                   blocks.GR_FILE_FLOAT: "float",
                   blocks.GR_FILE_DOUBLE: "double" }

ftype_to_size = {blocks.GR_FILE_BYTE: gr.sizeof_char,
                 blocks.GR_FILE_SHORT: gr.sizeof_short,
                 blocks.GR_FILE_INT: gr.sizeof_int,
                 blocks.GR_FILE_LONG: gr.sizeof_int,
                 blocks.GR_FILE_LONG_LONG: 2*gr.sizeof_int,
                 blocks.GR_FILE_FLOAT: gr.sizeof_float,
                 blocks.GR_FILE_DOUBLE: gr.sizeof_double}

def parse_header(p, VERBOSE=False):
    dump = pmt.PMT_NIL

    info = dict()

    if(pmt.is_dict(p) is False):
        sys.stderr.write("Header is not a PMT dictionary: invalid or corrupt data file.\n")
        sys.exit(1)

    # GET FILE FORMAT VERSION NUMBER
    if(pmt.dict_has_key(p, pmt.string_to_symbol("version"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("version"), dump)
        version = pmt.to_long(r)
        if(VERBOSE):
            print "Version Number: {0}".format(version)
    else:
        sys.stderr.write("Could not find key 'version': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT SAMPLE RATE
    if(pmt.dict_has_key(p, pmt.string_to_symbol("rx_rate"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("rx_rate"), dump)
        samp_rate = pmt.to_double(r)
        info["rx_rate"] = samp_rate
        if(VERBOSE):
            print "Sample Rate: {0:.2f} sps".format(samp_rate)
    else:
        sys.stderr.write("Could not find key 'sr': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT TIME STAMP
    if(pmt.dict_has_key(p, pmt.string_to_symbol("rx_time"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("rx_time"), dump)
        secs = pmt.tuple_ref(r, 0)
        fracs = pmt.tuple_ref(r, 1)
        secs = float(pmt.to_uint64(secs))
        fracs = pmt.to_double(fracs)
        t = secs + fracs
        info["rx_time"] = t
        if(VERBOSE):
            print "Seconds: {0:.6f}".format(t)
    else:
        sys.stderr.write("Could not find key 'time': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT ITEM SIZE
    if(pmt.dict_has_key(p, pmt.string_to_symbol("size"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("size"), dump)
        dsize = pmt.to_long(r)
        info["size"] = dsize
        if(VERBOSE):
            print "Item size: {0}".format(dsize)
    else:
        sys.stderr.write("Could not find key 'size': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT DATA TYPE
    if(pmt.dict_has_key(p, pmt.string_to_symbol("type"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("type"), dump)
        dtype = pmt.to_long(r)
        stype = ftype_to_string[dtype]
        info["type"] = stype
        if(VERBOSE):
            print "Data Type: {0} ({1})".format(stype, dtype)
    else:
        sys.stderr.write("Could not find key 'type': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT COMPLEX
    if(pmt.dict_has_key(p, pmt.string_to_symbol("cplx"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("cplx"), dump)
        cplx = pmt.to_bool(r)
        info["cplx"] = cplx
        if(VERBOSE):
            print "Complex? {0}".format(cplx)
    else:
        sys.stderr.write("Could not find key 'cplx': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT WHERE CURRENT SEGMENT STARTS
    if(pmt.dict_has_key(p, pmt.string_to_symbol("strt"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("strt"), dump)
        seg_start = pmt.to_uint64(r)
        info["hdr_len"] = seg_start
        info["extra_len"] = seg_start - HEADER_LENGTH
        info["has_extra"] = info["extra_len"] > 0
        if(VERBOSE):
            print "Header Length: {0} bytes".format(info["hdr_len"])
            print "Extra Length:  {0}".format((info["extra_len"]))
            print "Extra Header?  {0}".format(info["has_extra"])
    else:
        sys.stderr.write("Could not find key 'strt': invalid or corrupt data file.\n")
        sys.exit(1)

    # EXTRACT SIZE OF DATA
    if(pmt.dict_has_key(p, pmt.string_to_symbol("bytes"))):
        r = pmt.dict_ref(p, pmt.string_to_symbol("bytes"), dump)
        nbytes = pmt.to_uint64(r)

        nitems = nbytes/dsize
        info["nitems"] = nitems
        info["nbytes"] = nbytes

        if(VERBOSE):
            print "Size of Data: {0} bytes".format(nbytes)
            print "              {0} items".format(nitems)
    else:
        sys.stderr.write("Could not find key 'size': invalid or corrupt data file.\n")
        sys.exit(1)

    return info

# IF THERE IS EXTRA DATA, PULL OUT THE DICTIONARY AND PARSE IT
def parse_extra_dict(p, info, VERBOSE=False):
    if(pmt.is_dict(p) is False):
        sys.stderr.write("Extra header is not a PMT dictionary: invalid or corrupt data file.\n")
        sys.exit(1)

    items = pmt.dict_items(p)
    nitems = pmt.length(items)
    for i in xrange(nitems):
        item = pmt.nth(i, items)
        key = pmt.symbol_to_string(pmt.car(item))
        val = pmt.cdr(item)
        info[key] = val
        if(VERBOSE):
            print "{0}: {1}".format(key, val)

    return info
