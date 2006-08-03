#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, gr_unittest
import comedi

class qa_comedi (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None

    def test_000_nop (self):
        """Just see if we can import the module...
        They may not have COMEDI library, etc.  Don't try to run anything"""
        pass
    
if __name__ == '__main__':
    gr_unittest.main ()
