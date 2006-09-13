#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, gr_unittest
import ezdop

class qa_ezdop(gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None
"""
    def test_001_installed(self):
	DOP = ezdop.source_c();

    def test_002_start_stop(self):
	DOP = ezdop.source_c();
	DOP.set_rate(1000);
	print "rate =", DOP.rate()
	DOP.rotate();

	self.fg.connect(DOP, gr.null_sink(gr.sizeof_gr_complex))
	try:
	    self.fg.run()
	except KeyboardInterrupt:
	    pass
	DOP.stop_rotating();
"""

if __name__ == '__main__':
    gr_unittest.main ()
