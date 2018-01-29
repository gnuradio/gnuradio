#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks

import os
from os.path import getsize

g_in_file = os.path.join(os.getenv("srcdir"), "test_16bit_1chunk.wav")
g_extra_header_offset = 36
g_extra_header_len = 18

class test_wavefile(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_checkwavread(self):
	wf = blocks.wavfile_source(g_in_file)
	self.assertEqual(wf.sample_rate(), 8000)

    def test_002_checkwavcopy(self):
	infile  = g_in_file
	outfile = "test_out.wav"

	wf_in  = blocks.wavfile_source(infile)
	wf_out = blocks.wavfile_sink(outfile,
                                     wf_in.channels(),
                                     wf_in.sample_rate(),
                                     wf_in.bits_per_sample())
	self.tb.connect(wf_in, wf_out)
	self.tb.run()
	wf_out.close()

	# we're losing all extra header chunks
	self.assertEqual(getsize(infile) - g_extra_header_len, getsize(outfile))

	in_f  = file(infile,  'rb')
	out_f = file(outfile, 'rb')

	in_data  = in_f.read()
	out_data = out_f.read()
        out_f.close()
	os.remove(outfile)
	# cut extra header chunks input file
	self.assertEqual(in_data[:g_extra_header_offset] + \
	                 in_data[g_extra_header_offset + g_extra_header_len:], out_data)

if __name__ == '__main__':
    gr_unittest.run(test_wavefile, "test_wavefile.xml")
