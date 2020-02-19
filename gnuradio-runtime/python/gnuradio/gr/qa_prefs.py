#!/usr/bin/env python
#
# Copyright 2019,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function


from gnuradio import gr, gr_unittest

class test_prefs (gr_unittest.TestCase):

    def test_001(self):
        p = gr.prefs()

        # Read some options
        self.assertFalse(p.has_option('doesnt', 'exist'))

        # At the time these tests are run, there is not necessarily a default
        # configuration on the build system, so not much to do with testing here

if __name__ == '__main__':
    gr_unittest.run(test_prefs)
