#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

import Ice
import sys, time, random, numpy
from gnuradio import gr, gr_unittest, blocks

from gnuradio.ctrlport import GNURadio
from gnuradio import ctrlport
import os, struct

class test_ctrlport_probes(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'True'

    def tearDown(self):
        self.tb = None

    def test_001(self):
        data = range(1,9)

        self.src = blocks.vector_source_c(data, True)
        self.probe = blocks.ctrlport_probe2_c("samples","Complex",
                                              len(data), gr.DISPNULL)
        probe_name = self.probe.alias()

        self.tb.connect(self.src, self.probe)
        self.tb.start()

        # Probes return complex values as list of floats with re, im
        # Imaginary parts of this data set are 0.
        expected_result = [1, 0, 2, 0, 3, 0, 4, 0,
                           5, 0, 6, 0, 7, 0, 8, 0]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]

        # Initialize a simple Ice client from endpoint
        ic = Ice.initialize(sys.argv)
        base = ic.stringToProxy(ep)
        radio = GNURadio.ControlPortPrx.checkedCast(base)

        # Get all exported knobs
        ret = radio.get([probe_name + "::samples"])
        for name in ret.keys():
            # Get data in probe, which might be offset; find the
            # beginning and unwrap.
            result = ret[name].value
            i = result.index(1.0)
            result = result[i:] + result[0:i]
            self.assertEqual(expected_result, result)

        self.tb.stop()


    def test_002(self):
        data = range(1,9)

        self.src = blocks.vector_source_f(data, True)
        self.probe = blocks.ctrlport_probe2_f("samples","Floats",
                                              len(data), gr.DISPNULL)
        probe_name = self.probe.alias()

        self.tb.connect(self.src, self.probe)
        self.tb.start()

        expected_result = [1, 2, 3, 4, 5, 6, 7, 8,]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]

        # Initialize a simple Ice client from endpoint
        ic = Ice.initialize(sys.argv)
        base = ic.stringToProxy(ep)
        radio = GNURadio.ControlPortPrx.checkedCast(base)

        # Get all exported knobs
        ret = radio.get([probe_name + "::samples"])
        for name in ret.keys():
            # Get data in probe, which might be offset; find the
            # beginning and unwrap.
            result = ret[name].value
            i = result.index(1.0)
            result = result[i:] + result[0:i]
            self.assertEqual(expected_result, result)

        self.tb.stop()


    def test_003(self):
        data = range(1,9)

        self.src = blocks.vector_source_i(data, True)
        self.probe = blocks.ctrlport_probe2_i("samples","Integers",
                                              len(data), gr.DISPNULL)
        probe_name = self.probe.alias()

        self.tb.connect(self.src, self.probe)
        self.tb.start()

        expected_result = [1, 2, 3, 4, 5, 6, 7, 8,]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]

        # Initialize a simple Ice client from endpoint
        ic = Ice.initialize(sys.argv)
        base = ic.stringToProxy(ep)
        radio = GNURadio.ControlPortPrx.checkedCast(base)

        # Get all exported knobs
        ret = radio.get([probe_name + "::samples"])
        for name in ret.keys():
            # Get data in probe, which might be offset; find the
            # beginning and unwrap.
            result = ret[name].value
            i = result.index(1.0)
            result = result[i:] + result[0:i]
            self.assertEqual(expected_result, result)

        self.tb.stop()


    def test_004(self):
        data = range(1,9)

        self.src = blocks.vector_source_s(data, True)
        self.probe = blocks.ctrlport_probe2_s("samples","Shorts",
                                              len(data), gr.DISPNULL)
        probe_name = self.probe.alias()

        self.tb.connect(self.src, self.probe)
        self.tb.start()

        expected_result = [1, 2, 3, 4, 5, 6, 7, 8,]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]

        # Initialize a simple Ice client from endpoint
        ic = Ice.initialize(sys.argv)
        base = ic.stringToProxy(ep)
        radio = GNURadio.ControlPortPrx.checkedCast(base)

        # Get all exported knobs
        ret = radio.get([probe_name + "::samples"])
        for name in ret.keys():
            # Get data in probe, which might be offset; find the
            # beginning and unwrap.
            result = ret[name].value
            i = result.index(1.0)
            result = result[i:] + result[0:i]
            self.assertEqual(expected_result, result)

        self.tb.stop()

    def test_005(self):
        data = range(1,9)

        self.src = blocks.vector_source_b(data, True)
        self.probe = blocks.ctrlport_probe2_b("samples","Bytes",
                                              len(data), gr.DISPNULL)
        probe_name = self.probe.alias()

        self.tb.connect(self.src, self.probe)
        self.tb.start()

        expected_result = [1, 2, 3, 4, 5, 6, 7, 8,]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]

        # Initialize a simple Ice client from endpoint
        ic = Ice.initialize(sys.argv)
        base = ic.stringToProxy(ep)
        radio = GNURadio.ControlPortPrx.checkedCast(base)

        # Get all exported knobs
        ret = radio.get([probe_name + "::samples"])
        for name in ret.keys():
            # Get data in probe, which might be offset; find the
            # beginning and unwrap.
            result = ret[name].value
            result = list(struct.unpack(len(result)*'b', result))
            i = result.index(1)
            result = result[i:] + result[0:i]
            self.assertEqual(expected_result, result)

        self.tb.stop()

if __name__ == '__main__':
    gr_unittest.run(test_ctrlport_probes, "test_ctrlport_probes.xml")

