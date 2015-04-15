#!/usr/bin/env python
#
# Copyright 2012,2013,2015 Free Software Foundation, Inc.
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

#
# This program tests mixed python and c++ GRCP sets in a single app
#

import sys, time, random, numpy, re
from gnuradio import gr, gr_unittest, blocks

from gnuradio.ctrlport import GNURadio
from gnuradio import ctrlport
import os

class inc_class:
    def __init__(self,val):
        self.val = val;

    def _get(self):
        #print "returning get (val = %s)"%(str(self.val));
        return self.val;

    def _set(self,val):
        #print "updating val to %s"%(str(val));
        self.val = val;
        return;

getset1 = inc_class(10);
getset2 = inc_class(100.0);
getset3 = inc_class("test");

class test_cpp_py_binding_set(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'True'

    def tearDown(self):
        self.tb = None

    def test_001(self):

        g1 = gr.RPC_get_int("pyland", "v1", "unit_1_int",
                                  "Python Exported Int", 0, 100, 10,
                                  gr.DISPNULL)
        g1.activate(getset1._get)
        s1 = gr.RPC_get_int("pyland", "v1", "unit_1_int",
                                  "Python Exported Int", 0, 100, 10,
                                  gr.DISPNULL)
        s1.activate(getset1._set)
        time.sleep(0.01)

        # test int variables
        getset1._set(21)
        val = getset1._get()
        rval = g1.get()
        self.assertEqual(val, rval)

        g2 = gr.RPC_get_float("pyland", "v2", "unit_2_float",
                              "Python Exported Float", -100, 1000.0, 100.0,
                              gr.DISPNULL)
        g2.activate(getset2._get)
        s2 = gr.RPC_get_float("pyland", "v2", "unit_2_float",
                              "Python Exported Float", -100, 1000.0, 100.0,
                              gr.DISPNULL)
        s2.activate(getset2._set)
        time.sleep(0.01)

        # test float variables
        getset2._set(123.456)
        val = getset2._get()
        rval = g2.get()
        self.assertAlmostEqual(val, rval, 4)

        g3 = gr.RPC_get_string("pyland", "v3", "unit_3_string",
                               "Python Exported String", "", "", "",
                               gr.DISPNULL)
        g3.activate(getset3._get)
        s3 = gr.RPC_get_string("pyland", "v3", "unit_3_string",
                               "Python Exported String", "", "", "",
                               gr.DISPNULL)
        s3.activate(getset3._set)
        time.sleep(0.01)

        # test string variables
        getset3._set("third test")
        val = getset3._get()
        rval = g3.get()
        self.assertEqual(val, rval)


    def test_002(self):
        data = range(1, 10)

        self.src = blocks.vector_source_c(data, True)
        self.p = blocks.nop(gr.sizeof_gr_complex)
        self.p.set_ctrlport_test(0);
        probe_info = self.p.alias()

        self.tb.connect(self.src, self.p)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]
        hostname = re.search("-h (\S+|\d+\.\d+\.\d+\.\d+)", ep).group(1)
        portnum = re.search("-p (\d+)", ep).group(1)
        argv = [None, hostname, portnum]

        # Initialize a simple ControlPort client from endpoint
        from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient
        radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
        radio = radiosys.client

        self.tb.start()

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get all exported knobs
        key_name_test = probe_info+"::test"
        ret = radio.getKnobs([key_name_test,])

        ret[key_name_test].value = 10
        radio.setKnobs({key_name_test: ret[key_name_test]})

        ret = radio.getKnobs([])
        result_test = ret[key_name_test].value
        self.assertEqual(result_test, 10)

        self.tb.stop()
        self.tb.wait()

if __name__ == '__main__':
    gr_unittest.run(test_cpp_py_binding_set, "test_cpp_py_binding_set.xml")
