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
# This program tests mixed python and c++ ctrlport exports in a single app
#

import sys, time, random, numpy, re
from gnuradio import gr, gr_unittest, blocks

from gnuradio.ctrlport import GNURadio
from gnuradio import ctrlport
import os

def get1():
    return "success"

def get2():
    return "failure"

class inc_class:
    def __init__(self):
        self.val = 1
    def pp(self):
        self.val = self.val+1
        return self.val

get3 = inc_class()

def get4():
    random.seed(0)
    rv = random.random()
    return rv

def get5():
    numpy.random.seed(0)
    samp_t = numpy.random.randn(24)+1j*numpy.random.randn(24);
    samp_f = numpy.fft.fft(samp_t);
    log_pow_f = 20*numpy.log10(numpy.abs(samp_f))
    rv = list(log_pow_f)
    return rv;

def get6():
    numpy.random.seed(0)
    samp_t = numpy.random.randn(1024)+1j*numpy.random.randn(1024);
    rv = list(samp_t)
    return rv;

class test_cpp_py_binding(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'True'

    def tearDown(self):
        self.tb = None

    def test_001(self):
        v1 = gr.RPC_get_string("pyland", "v1", "unit_1_string",
                               "Python Exported String", "", "", "",
                               gr.DISPNULL)
        v1.activate(get1)

        v2 = gr.RPC_get_string("pyland", "v2", "unit_2_string",
                               "Python Exported String", "", "", "",
                               gr.DISPNULL)
        v2.activate(get2)

        v3 = gr.RPC_get_int("pyland", "v3", "unit_3_int",
                            "Python Exported Int", 0, 100, 1,
                            gr.DISPNULL)
        v3.activate(get3.pp)

        v4 = gr.RPC_get_double("pyland", "time", "unit_4_time_double",
                               "Python Exported Double", 0, 1000, 1,
                               gr.DISPNULL)
        v4.activate(get4)

        v5 = gr.RPC_get_vector_float("pyland", "fvec", "unit_5_float_vector",
                                     "Python Exported Float Vector", [], [], [],
                                     gr.DISPTIME | gr.DISPOPTCPLX)
        v5.activate(get5)

        v6 = gr.RPC_get_vector_gr_complex("pyland", "cvec", "unit_6_gr_complex_vector",
                                          "Python Exported Complex Vector", [], [], [],
                                          gr.DISPXY | gr.DISPOPTSCATTER)
        v6.activate(get6)

        # print some variables locally
        val = get1()
        rval = v1.get()
        self.assertEqual(val, rval)

        val = get2()
        rval = v2.get()
        self.assertEqual(val, rval)

        val = get3.pp()
        rval = v3.get()
        self.assertEqual(val+1, rval)

        val = get4()
        rval = v4.get()
        self.assertEqual(val, rval)

        val = get5()
        rval = v5.get()
        self.assertComplexTuplesAlmostEqual(val, rval, 5)

        val = get6()
        rval = v6.get()
        self.assertComplexTuplesAlmostEqual(val, rval, 5)

    def test_002(self):
        data = range(1,9)

        self.src = blocks.vector_source_c(data)
        self.p1 = blocks.ctrlport_probe_c("aaa","C++ exported variable")
        self.p2 = blocks.ctrlport_probe_c("bbb","C++ exported variable")
        probe_name = self.p2.alias()

        self.tb.connect(self.src, self.p1)
        self.tb.connect(self.src, self.p2)
        self.tb.start()

        # Probes return complex values as list of floats with re, im
        # Imaginary parts of this data set are 0.
        expected_result = [1, 2, 3, 4,
                           5, 6, 7, 8]

        # Make sure we have time for flowgraph to run
        time.sleep(0.1)

        # Get available endpoint
        ep = gr.rpcmanager_get().endpoints()[0]
        hostname = re.search("-h (\S+|\d+\.\d+\.\d+\.\d+)", ep).group(1)
        portnum = re.search("-p (\d+)", ep).group(1)
        argv = [None, hostname, portnum]

        # Initialize a simple ControlPort client from endpoint
        from gnuradio.ctrlport.GNURadioControlPortClient import GNURadioControlPortClient
        radiosys = GNURadioControlPortClient(argv=argv, rpcmethod='thrift')
        radio = radiosys.client

        # Get all exported knobs
        ret = radio.getKnobs([probe_name + "::bbb"])
        for name in ret.keys():
            result = ret[name].value
            self.assertEqual(result, expected_result)

        self.tb.stop()

if __name__ == '__main__':
    gr_unittest.run(test_cpp_py_binding, "test_cpp_py_binding.xml")
