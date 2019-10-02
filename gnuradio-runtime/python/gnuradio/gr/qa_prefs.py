#!/usr/bin/env python
#
# Copyright 2019,2020 Free Software Foundation, Inc.
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
