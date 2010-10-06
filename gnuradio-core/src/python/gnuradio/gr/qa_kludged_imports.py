#!/usr/bin/env python
#
# Copyright 2005,2008,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest

class test_kludged_imports (gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_blks_import(self):
        # make sure that this somewhat magic import works
        from gnuradio import blks2

    def test_gru_import(self):
        # make sure that this somewhat magic import works
        from gnuradio import gru


if __name__ == '__main__':
    gr_unittest.run(test_kludged_imports, "test_kludged_imports.xml")
