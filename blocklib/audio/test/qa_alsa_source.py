#!/usr/bin/env python3
#
# Copyright 2022 Volker Schroer, dl1ksv
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, audio

class test_alsa_source(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_instantiate(self):
        # If this is a templated block, be sure to add the appropriate suffix
        op = audio.alsa_source(48000,"hw:0")

    def test_give_descriptive_name(self):
        # Set up a flowgraph, run, validate the results
        pass


if __name__ == '__main__':
    gr_unittest.run(test_alsa_source)

