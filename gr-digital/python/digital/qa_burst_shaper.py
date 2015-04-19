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

def make_length_tag(offset, length):
    return gr.python_to_tag({'offset' : offset,
                             'key' : pmt.intern('packet_len'),
                             'value' : pmt.from_long(length),
                             'srcid' : pmt.intern('qa_burst_shaper')})

def compare_tags(a, b):
    a = gr.tag_to_python(a)
    b = gr.tag_to_python(b)
    return a.key == b.key and a.offset == b.offset and \
           a.value == b.value
    #return a.key == b.key and a.offset == b.offset and \
    #       a.srcid == b.srcid and a.value == b.value

class qa_burst_shaper (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_ff (self):
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length + 10) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length + 10,
                       dtype=complex) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length + 10) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length + 10,
                       dtype=complex) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(length + 10) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 9
        data = np.ones(length + 10) # need 10 more to push things through
        window = np.concatenate((-2.0*np.ones(5), -3.0*np.ones(1),
                                 -4.0*np.ones(5)))
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
        prepad = 10
        postpad = 10
        length1 = 15
        length2 = 25
        data = np.concatenate((np.ones(length1), -1.0*np.ones(length2),
                               np.zeros(10))) # need 10 more to push things through
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
        prepad = 10
        postpad = 10
        length = 20
        data = np.ones(2*length + 10) # need 10 more to push things through
        window = np.concatenate((-2.0*np.ones(5), -4.0*np.ones(5)))
        tags = (make_length_tag(0, length), make_length_tag(length + 5, length))
        expected = np.concatenate((np.zeros(prepad), window[0:5],
                                   np.ones(length - len(window)), window[5:10],
                                   np.zeros(postpad)))
        etags = (make_length_tag(0, length + prepad + postpad),
                 make_length_tag(length + prepad + postpad,
                                 length + prepad + postpad))

        # flowgraph
        source = blocks.vector_source_f(data, tags=tags)
        shaper = digital.burst_shaper_ff(window, pre_padding=prepad,
                                         post_padding=postpad)
        sink = blocks.vector_sink_f()
        self.tb.connect(source, shaper, sink)
        self.tb.run ()

        # checks
        self.assertFloatTuplesAlmostEqual(sink.data(),
                                          np.concatenate((expected, expected),
                                          6))
        for i in xrange(len(etags)):
            self.assertTrue(compare_tags(sink.tags()[i], etags[i]))


if __name__ == '__main__':
    gr_unittest.run(qa_burst_shaper, "qa_burst_shaper.xml")
