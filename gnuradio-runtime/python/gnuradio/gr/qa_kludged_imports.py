#!/usr/bin/env python
#
# Copyright 2005,2008,2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest

class test_kludged_imports (gr_unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_gru_import(self):
        # make sure that this somewhat magic import works
        from gnuradio import gru


if __name__ == '__main__':
    gr_unittest.run(test_kludged_imports, "test_kludged_imports.xml")
