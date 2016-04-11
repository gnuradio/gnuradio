#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 Free Software Foundation, Inc.
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
from gnuradio import blocks, digital
import pmt
import numpy as np
import sys

def make_length_tag(offset, length):
    return gr.python_to_tag({'offset' : offset,
                             'key' : pmt.intern('packet_len'),
                             'value' : pmt.from_long(length),
                             'srcid' : pmt.intern('qa_burst_shaper')})

def make_tag(offset, key, value):
    return gr.python_to_tag({'offset' : offset,
                             'key' : pmt.intern(key),
                             'value' : value,
                             'srcid' : pmt.intern('qa_burst_shaper')})

def compare_tags(a, b):
    return a.offset == b.offset and pmt.equal(a.key, b.key) and \
           pmt.equal(a.value, b.value)

class qa_burst_shaper (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_ff (self):
        '''
        test_ff: test with float values, even length window, zero padding,
            and no phasing
        '''
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length)
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        tags = (make_length_tag(0, length),)
        expected = np.concatenate((np.zeros(prepad), window[0:5],
                                   np.ones(length - len(window)), window[5:10],
                                   np.zeros(postpad)))
        etag = make_length_tag(0, length + prepad + postpad)

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_cc (self):
        '''
        test_cc: test with complex values, even length window, zero padding,
            and no phasing
        '''
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length, dtype=complex)
        window = np.concatenate((-2.0*np.ones(5, dtype=complex),
                                 -4.0*np.ones(5, dtype=complex)))
        tags = (make_length_tag(0, length),)
        expected = np.concatenate((np.zeros(prepad, dtype=complex), window[0:5],
                                   np.ones(length - len(window), dtype=complex),
                                   window[5:10], np.zeros(postpad,
                                   dtype=complex)))
        etag = make_length_tag(0, length + prepad + postpad)

        # flowgraph
        source = blocks.vector_source_c(data, tags=tags)
        shaper = digital.burst_shaper_cc(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_c()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertComplexTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_ff_with_phasing (self):
        '''
        test_ff_with_phasing: test with float values, even length window, zero
            padding, and phasing
        '''
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length)
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        tags = (make_length_tag(0, length),)
        phasing = np.zeros(5)
        for i in xrange(5):
            phasing[i] = ((-1.0)**i)
        expected = np.concatenate((np.zeros(prepad), phasing*window[0:5],
                                   np.ones(length), phasing*window[5:10],
                                   np.zeros(postpad)))
        etag = make_length_tag(0, length + prepad + postpad + len(window))

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad,
                                         insert_phasing=True)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_cc_with_phasing (self):
        '''
        test_cc_with_phasing: test with complex values, even length window, zero
            padding, and phasing
        '''
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length, dtype=complex)
        window = np.concatenate((-2.0*np.ones(5, dtype=complex),
                                 -4.0*np.ones(5, dtype=complex)))
        tags = (make_length_tag(0, length),)
        phasing = np.zeros(5, dtype=complex)
        for i in xrange(5):
            phasing[i] = complex((-1.0)**i)
        expected = np.concatenate((np.zeros(prepad, dtype=complex),
                                   phasing*window[0:5],
                                   np.ones(length, dtype=complex),
                                   phasing*window[5:10],
                                   np.zeros(postpad, dtype=complex)))
        etag = make_length_tag(0, length + prepad + postpad + len(window))

        # flowgraph
        source = blocks.vector_source_c(data, tags=tags)
        shaper = digital.burst_shaper_cc(window, pre_padding=prepad,
                                         post_padding=postpad,
                                         insert_phasing=True)
        sink = blocks.vector_sink_c()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertComplexTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_odd_window (self):
        '''
        test_odd_window: test with odd length window; center sample should be
            applied at end of up flank and beginning of down flank
        '''
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length)
        window = np.concatenate((-2.0*np.ones(5), -3.0*np.ones(1),
                                 -4.0*np.ones(5)))
        tags = (make_length_tag(0, length),)
        expected = np.concatenate((np.zeros(prepad), window[0:6],
                                   np.ones(length - len(window) - 1),
                                   window[5:11], np.zeros(postpad)))
        etag = make_length_tag(0, length + prepad + postpad)

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_short_burst (self):
        '''
        test_short_burst: test with burst length shorter than window length;
            clips the window up and down flanks to FLOOR(length/2) samples
        '''
        prepad = 10
        postpad = 10
        length = 9
        data = np.ones(length)
        window = np.arange(length + 2, dtype=float)
        tags = (make_length_tag(0, length),)
        expected = np.concatenate((np.zeros(prepad), window[0:4],
                                   np.ones(1), window[5:9],
                                   np.zeros(postpad)))
        etag = make_length_tag(0, length + prepad + postpad)

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        self.assertTrue(compare_tags(sink.tags()[0], etag))

    def test_consecutive_bursts (self):
        '''
        test_consecutive_bursts: test with consecutive bursts of different
            lengths
        '''
        prepad = 10
        postpad = 10
        length1 = 15
        length2 = 25
        data = np.concatenate((np.ones(length1), -1.0*np.ones(length2)))
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        tags = (make_length_tag(0, length1), make_length_tag(length1, length2))
        expected = np.concatenate((np.zeros(prepad), window[0:5],
                                   np.ones(length1 - len(window)), window[5:10],
                                   np.zeros(postpad + prepad), -1.0*window[0:5],
                                   -1.0*np.ones(length2 - len(window)),
                                   -1.0*window[5:10], np.zeros(postpad)))
        etags = (make_length_tag(0, length1 + prepad + postpad),
                 make_length_tag(length1 + prepad + postpad,
                                 length2 + prepad + postpad))

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        for i in xrange(len(etags)):
            self.assertTrue(compare_tags(sink.tags()[i], etags[i]))

    def test_tag_gap (self):
        '''
        test_tag_gap: test with gap between tags; should drop samples that are
            between proper tagged streams
        '''
        prepad = 10
        postpad = 10
        length = 20
        gap_len = 5
        data = np.arange(2*length + gap_len, dtype=float)
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        ewindow = window * np.array([1,-1,1,-1,1,1,-1,1,-1,1],dtype=float)
        tags = (make_length_tag(0, length),
                make_length_tag(length + gap_len, length))
        expected = np.concatenate((np.zeros(prepad), ewindow[0:5],
                                   np.arange(0, length, dtype=float),
                                   ewindow[5:10], np.zeros(postpad),
                                   np.zeros(prepad), ewindow[0:5],
                                   np.arange(length + gap_len,
                                             2*length + gap_len, dtype=float),
                                   ewindow[5:10], np.zeros(postpad)))
        burst_len = length + len(window) + prepad + postpad
        etags = (make_length_tag(0, burst_len),
                 make_length_tag(burst_len, burst_len))

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad,
                                         insert_phasing=True)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        for i in xrange(len(etags)):
            self.assertTrue(compare_tags(sink.tags()[i], etags[i]))

    def test_tag_propagation (self):
        '''
        test_tag_propagation: test that non length tags are handled correctly
        '''
        prepad = 10
        postpad = 10
        length1 = 15
        length2 = 25
        gap_len = 5
        lentag1_offset = 0
        lentag2_offset = length1 + gap_len
        tag1_offset = 0                     # accompanies first length tag
        tag2_offset = length1 + gap_len     # accompanies second length tag
        tag3_offset = 2                     # in ramp-up state
        tag4_offset = length1 + 2           # in gap; tag will be dropped
        tag5_offset = length1 + gap_len + 7 # in copy state

        data = np.concatenate((np.ones(length1), np.zeros(gap_len),
                               -1.0*np.ones(length2)))
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        tags = (make_length_tag(lentag1_offset, length1),
                make_length_tag(lentag2_offset, length2),
                make_tag(tag1_offset, 'head', pmt.intern('tag1')),
                make_tag(tag2_offset, 'head', pmt.intern('tag2')),
                make_tag(tag3_offset, 'body', pmt.intern('tag3')),
                make_tag(tag4_offset, 'body', pmt.intern('tag4')),
                make_tag(tag5_offset, 'body', pmt.intern('tag5')))
        expected = np.concatenate((np.zeros(prepad), window[0:5],
                                   np.ones(length1 - len(window)), window[5:10],
                                   np.zeros(postpad + prepad), -1.0*window[0:5],
                                   -1.0*np.ones(length2 - len(window)),
                                   -1.0*window[5:10], np.zeros(postpad)))
        elentag1_offset = 0
        elentag2_offset = length1 + prepad + postpad
        etag1_offset = 0
        etag2_offset = elentag2_offset
        etag3_offset = prepad + tag3_offset
        etag5_offset = 2*prepad + postpad + tag5_offset - gap_len
        etags = (make_length_tag(elentag1_offset, length1 + prepad + postpad),
                 make_length_tag(elentag2_offset, length2 + prepad + postpad),
                 make_tag(etag1_offset, 'head', pmt.intern('tag1')),
                 make_tag(etag2_offset, 'head', pmt.intern('tag2')),
                 make_tag(etag3_offset, 'body', pmt.intern('tag3')),
                 make_tag(etag5_offset, 'body', pmt.intern('tag5')))

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(), expected, 6)
        for x, y in zip(sorted(sink.tags(), key=gr.tag_t_offset_compare_key()),
                        sorted(etags, key=gr.tag_t_offset_compare_key())):
            self.assertTrue(compare_tags(x, y))


if __name__ == '__main__':
    gr_unittest.run(qa_burst_shaper, "qa_burst_shaper.xml")
