#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2010,2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, filter, blocks

import sys
import random

def make_random_complex_tuple(L):
    result = []
    for x in range(L):
        result.append(complex(2*random.random()-1,
                              2*random.random()-1))
    return tuple(result)

def make_random_float_tuple(L):
    result = []
    for x in range(L):
        result.append(float(int(2*random.random()-1)))
    return tuple(result)


def reference_filter_ccc(dec, taps, input):
    """
    compute result using conventional fir filter
    """
    tb = gr.top_block()
    #src = blocks.vector_source_c(((0,) * (len(taps) - 1)) + input)
    src = blocks.vector_source_c(input)
    op = filter.fir_filter_ccc(dec, taps)
    dst = blocks.vector_sink_c()
    tb.connect(src, op, dst)
    tb.run()
    return dst.data()

def reference_filter_fff(dec, taps, input):
    """
    compute result using conventional fir filter
    """
    tb = gr.top_block()
    #src = blocks.vector_source_f(((0,) * (len(taps) - 1)) + input)
    src = blocks.vector_source_f(input)
    op = filter.fir_filter_fff(dec, taps)
    dst = blocks.vector_sink_f()
    tb.connect(src, op, dst)
    tb.run()
    return dst.data()

def reference_filter_ccf(dec, taps, input):
    """
    compute result using conventional fir filter
    """
    tb = gr.top_block()
    src = blocks.vector_source_c(input)
    op = filter.fir_filter_ccf(dec, taps)
    dst = blocks.vector_sink_c()
    tb.connect(src, op, dst)
    tb.run()
    return dst.data()


def print_complex(x):
    for i in x:
        i = complex(i)
        sys.stdout.write("(%6.3f,%6.3fj), " % (i.real, i.imag))
    sys.stdout.write('\n')


class test_fft_filter(gr_unittest.TestCase):

    def setUp(self):
	pass

    def tearDown(self):
	pass

    def assert_fft_ok2(self, expected_result, result_data):
        expected_result = expected_result[:len(result_data)]
        self.assertComplexTuplesAlmostEqual2 (expected_result, result_data,
                                              abs_eps=1e-9, rel_eps=4e-4)

    def assert_fft_float_ok2(self, expected_result, result_data, abs_eps=1e-9, rel_eps=4e-4):
        expected_result = expected_result[:len(result_data)]
        self.assertFloatTuplesAlmostEqual2 (expected_result, result_data,
                                            abs_eps, rel_eps)

    def test_ccc_001(self):
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (1,)
        expected_result = tuple([complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op =  filter.fft_filter_ccc(1, taps)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)


    def test_ccc_002(self):
        # Test nthreads
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        nthreads = 2
        expected_result = tuple([2 * complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op = filter.fft_filter_ccc(1, taps, nthreads)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)

    def test_ccc_003(self):
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        expected_result = tuple([2 * complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op = filter.fft_filter_ccc(1, taps)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)


    def test_ccc_004(self):
        random.seed(0)
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 1000))
            taps = make_random_complex_tuple(ntaps)
            expected_result = reference_filter_ccc(1, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccc(1, taps)
            dst = blocks.vector_sink_c()
	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()
	    del tb
            self.assert_fft_ok2(expected_result, result_data)

    def test_ccc_005(self):
        random.seed(0)
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_complex_tuple(ntaps)
            expected_result = reference_filter_ccc(dec, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccc(dec, taps)
            dst = blocks.vector_sink_c()
            tb = gr.top_block()
	    tb.connect(src, op, dst)
            tb.run()
            del tb
	    result_data = dst.data()

            self.assert_fft_ok2(expected_result, result_data)

    def test_ccc_006(self):
        # Test decimating with nthreads=2
        random.seed(0)
        nthreads = 2
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_complex_tuple(ntaps)
            expected_result = reference_filter_ccc(dec, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccc(dec, taps, nthreads)
            dst = blocks.vector_sink_c()
            tb = gr.top_block()
	    tb.connect(src, op, dst)
            tb.run()
            del tb
	    result_data = dst.data()

            self.assert_fft_ok2(expected_result, result_data)

    # ----------------------------------------------------------------
    # test _ccf version
    # ----------------------------------------------------------------

    def test_ccf_001(self):
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (1,)
        expected_result = tuple([complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op =  filter.fft_filter_ccf(1, taps)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)


    def test_ccf_002(self):
        # Test nthreads
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        nthreads = 2
        expected_result = tuple([2 * complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op = filter.fft_filter_ccf(1, taps, nthreads)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)

    def test_ccf_003(self):
	tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        expected_result = tuple([2 * complex(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_c(src_data)
        op = filter.fft_filter_ccf(1, taps)
        dst = blocks.vector_sink_c()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)


    def test_ccf_004(self):
        random.seed(0)
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 1000))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_ccf(1, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccf(1, taps)
            dst = blocks.vector_sink_c()
	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()
	    del tb
            self.assert_fft_ok2(expected_result, result_data)

    def test_ccf_005(self):
        random.seed(0)
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_ccf(dec, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccf(dec, taps)
            dst = blocks.vector_sink_c()
            tb = gr.top_block()
	    tb.connect(src, op, dst)
            tb.run()
            del tb
	    result_data = dst.data()

            self.assert_fft_ok2(expected_result, result_data)

    def test_ccf_006(self):
        # Test decimating with nthreads=2
        random.seed(0)
        nthreads = 2
        for i in xrange(25):
            # sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_complex_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_ccf(dec, taps, src_data)

            src = blocks.vector_source_c(src_data)
            op = filter.fft_filter_ccc(dec, taps, nthreads)
            dst = blocks.vector_sink_c()
            tb = gr.top_block()
	    tb.connect(src, op, dst)
            tb.run()
            del tb
	    result_data = dst.data()

            self.assert_fft_ok2(expected_result, result_data)

    # ----------------------------------------------------------------
    # test _fff version
    # ----------------------------------------------------------------

    def test_fff_001(self):
        tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (1,)
        expected_result = tuple([float(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_f(src_data)
        op = filter.fft_filter_fff(1, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 5)


    def test_fff_002(self):
        tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        expected_result = tuple([2 * float(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_f(src_data)
        op = filter.fft_filter_fff(1, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        #print 'expected:', expected_result
        #print 'results: ', result_data
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 5)

    def test_fff_003(self):
        # Test 02 with nthreads
        tb = gr.top_block()
        src_data = (0,1,2,3,4,5,6,7)
        taps = (2,)
        nthreads = 2
        expected_result = tuple([2 * float(x) for x in (0,1,2,3,4,5,6,7)])
        src = blocks.vector_source_f(src_data)
        op = filter.fft_filter_fff(1, taps, nthreads)
        dst = blocks.vector_sink_f()
        tb.connect(src, op, dst)
        tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 5)

    def xtest_fff_004(self):
        random.seed(0)
        for i in xrange(25):
            sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            src_len = 4096
            src_data = make_random_float_tuple(src_len)
            ntaps = int(random.uniform(2, 1000))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_fff(1, taps, src_data)

            src = blocks.vector_source_f(src_data)
            op = filter.fft_filter_fff(1, taps)
            dst = blocks.vector_sink_f()
    	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()

            #print "src_len =", src_len, " ntaps =", ntaps
            try:
                self.assert_fft_float_ok2(expected_result, result_data, abs_eps=1.0)
            except:
                expected = open('expected', 'w')
                for x in expected_result:
                    expected.write(`x` + '\n')
                actual = open('actual', 'w')
                for x in result_data:
                    actual.write(`x` + '\n')
                raise

    def xtest_fff_005(self):
        random.seed(0)
        for i in xrange(25):
            sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            src_len = 4*1024
            src_data = make_random_float_tuple(src_len)
            ntaps = int(random.uniform(2, 1000))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_fff(1, taps, src_data)

            src = blocks.vector_source_f(src_data)
            op = filter.fft_filter_fff(1, taps)
            dst = blocks.vector_sink_f()
    	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()

            self.assert_fft_float_ok2(expected_result, result_data, abs_eps=2.0)

    def xtest_fff_006(self):
        random.seed(0)
        for i in xrange(25):
            sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_float_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_fff(dec, taps, src_data)

            src = blocks.vector_source_f(src_data)
            op = filter.fft_filter_fff(dec, taps)
            dst = blocks.vector_sink_f()
    	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()

            self.assert_fft_float_ok2(expected_result, result_data)

    def xtest_fff_007(self):
        # test decimation with nthreads
        random.seed(0)
        nthreads = 2
        for i in xrange(25):
            sys.stderr.write("\n>>> Loop = %d\n" % (i,))
            dec = i + 1
            src_len = 4*1024
            src_data = make_random_float_tuple(src_len)
            ntaps = int(random.uniform(2, 100))
            taps = make_random_float_tuple(ntaps)
            expected_result = reference_filter_fff(dec, taps, src_data)

            src = blocks.vector_source_f(src_data)
            op = filter.fft_filter_fff(dec, taps, nthreads)
            dst = blocks.vector_sink_f()
    	    tb = gr.top_block()
            tb.connect(src, op, dst)
            tb.run()
            result_data = dst.data()

            self.assert_fft_float_ok2(expected_result, result_data)

    def test_fff_get0(self):
        random.seed(0)
        for i in xrange(25):
            ntaps = int(random.uniform(2, 100))
            taps = make_random_float_tuple(ntaps)

            op = filter.fft_filter_fff(1, taps)
            result_data = op.taps()
            #print result_data

            self.assertEqual(taps, result_data)

    def test_ccc_get0(self):
        random.seed(0)
        for i in xrange(25):
            ntaps = int(random.uniform(2, 100))
            taps = make_random_complex_tuple(ntaps)

            op = filter.fft_filter_ccc(1, taps)
            result_data = op.taps()
            #print result_data

            self.assertComplexTuplesAlmostEqual(taps, result_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_fft_filter, "test_fft_filter.xml")

