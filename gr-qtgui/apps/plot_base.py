#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

from gnuradio import gr, blocks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import os, sys

os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'

try:
    from gnuradio import qtgui
    from PyQt4 import QtGui, QtCore
    import sip
except ImportError:
    print "Error: Program requires PyQt4 and gr-qtgui."
    sys.exit(1)

try:
    import scipy
except ImportError:
    print "Error: Scipy required (www.scipy.org)."
    sys.exit(1)

try:
    from gnuradio.qtgui.plot_constellation_form import *
    from gnuradio.qtgui.plot_psd_form import *
    from gnuradio.qtgui.plot_spectrogram_form import *
    from gnuradio.qtgui.plot_time_form import *
    from gnuradio.qtgui.plot_time_raster_form import *
except ImportError:
    from plot_constellation_form import *
    from plot_psd_form import *
    from plot_spectrogram_form import *
    from plot_time_form import *
    from plot_time_raster_form import *

def read_samples(filename, start, in_size, min_size, dtype, dtype_size):
    # Read in_size number of samples from file
    fhandle = open(filename, 'r')
    fhandle.seek(start*dtype_size, 0)
    data = scipy.fromfile(fhandle, dtype=dtype, count=in_size)
    data_min = 1.1*data.min()
    data_max = 1.1*data.max()
    data = data.tolist()
    fhandle.close()

    if(min_size > 0):
        if(len(data) < in_size):
            print "Warning: read in {0} samples but asked for {1} samples.".format(
                len(data), in_size)
    else:
        # If we have to, append 0's to create min_size samples of data
        if(len(data) < min_size):
            data += (min_size - len(data)) * [dtype(0)]


    return data, data_min, data_max

def read_samples_f(filename, start, in_size, min_size=0):
    return read_samples(filename, start, in_size, min_size,
                        scipy.float32, gr.sizeof_float)

def read_samples_i(filename, start, in_size, min_size=0):
    return read_samples(filename, start, in_size, min_size,
                        scipy.int32, gr.sizeof_int)

def read_samples_s(filename, start, in_size, min_size=0):
    return read_samples(filename, start, in_size, min_size,
                        scipy.int16, gr.sizeof_short)

def read_samples_b(filename, start, in_size, min_size=0):
    d,mn,mx = read_samples(filename, start, in_size, min_size,
                           scipy.int8, gr.sizeof_char)

    # Bit of a hack since we want to read the data as signed ints, but
    # the blocks.vector_source_b will only accept unsigned. We read in as
    # signed, do our min/max and things on that, then convert here.
    d = scipy.array(d, dtype=scipy.uint8).tolist()
    return d,mn,mx

def read_samples_c(filename, start, in_size, min_size=0):
    # Complex samples are handled differently
    fhandle = open(filename, 'r')
    fhandle.seek(start*gr.sizeof_gr_complex, 0)
    data = scipy.fromfile(fhandle, dtype=scipy.complex64, count=in_size)
    data_min = 1.1*float(min(data.real.min(), data.imag.min()))
    data_max = 1.1*float(max(data.real.max(), data.imag.max()))
    data = data.tolist()
    fhandle.close()

    if(min_size > 0):
        if(len(data) < in_size):
            print "Warning: read in {0} samples but asked for {1} samples.".format(
                len(data), in_size)
    else:
        # If we have to, append 0's to create min_size samples of data
        if(len(data) < min_size):
            data += (min_size - len(data)) * [complex(0,0)]

    return data, data_min, data_max

class source_ints_to_float(gr.hier_block2):
    def __init__(self, data):
	gr.hier_block2.__init__(self, "ints_to_floats",
				gr.io_signature(0, 0, 0),
				gr.io_signature(1, 1, gr.sizeof_float))
        self.src = blocks.vector_source_i(data)
        self.cvt = blocks.int_to_float()
        self.connect(self.src, self.cvt, self)

    def set_data(self, data):
        self.src.set_data(data)

class source_shorts_to_float(gr.hier_block2):
    def __init__(self, data):
	gr.hier_block2.__init__(self, "shorts_to_floats",
				gr.io_signature(0, 0, 0),
				gr.io_signature(1, 1, gr.sizeof_float))
        self.src = blocks.vector_source_s(data)
        self.cvt = blocks.short_to_float()
        self.connect(self.src, self.cvt, self)

    def set_data(self, data):
        self.src.set_data(data)

class source_chars_to_float(gr.hier_block2):
    def __init__(self, data):
	gr.hier_block2.__init__(self, "chars_to_floats",
				gr.io_signature(0, 0, 0),
				gr.io_signature(1, 1, gr.sizeof_float))
        self.src = blocks.vector_source_b(data)
        self.cvt = blocks.char_to_float()
        self.connect(self.src, self.cvt, self)

    def set_data(self, data):
        self.src.set_data(data)

def find_max_nsamples(filelist):
    # Find the smallest number of samples in all files and use that as
    # a maximum value possible.
    filesizes = []
    for f in filelist:
        if(os.path.exists(f)):
            filesizes.append(os.path.getsize(f) / gr.sizeof_gr_complex)
    max_nsamples = min(filesizes)
    return max_nsamples
