#!/usr/bin/env python
#
# Copyright 2006,2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, video_sdl

class test_video_sdl (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_000_nop (self):
        """Just see if we can import the module...
        They may not have video drivers, etc.  Don't try to run anything"""
        pass

if __name__ == '__main__':
    gr_unittest.run(test_video_sdl, "test_video_sdl.xml")
