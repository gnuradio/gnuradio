#!/usr/bin/env python3
#
# Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, newmod

class test_newblock(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_instantiate(self):
        # If this is a templated block, be sure to add the appropriate suffix
        op = newmod.newblock()

    def test_give_descriptive_name(self):
        # Set up a flowgraph, run, validate the results
        pass


if __name__ == '__main__':
    gr_unittest.run(test_newblock)

