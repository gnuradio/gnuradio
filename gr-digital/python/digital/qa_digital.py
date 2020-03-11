#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 


from gnuradio import gr, gr_unittest, digital

class test_digital(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

if __name__ == '__main__':
    gr_unittest.run(test_digital, "test_digital.xml")
