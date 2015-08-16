#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 Free Software Foundation, Inc.
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

import time
import numpy
import os
import pmt
from gnuradio import gr, gr_unittest
from gnuradio import blocks

BLOCK_LOOKUP = {
    'float': {
        'src':  blocks.vector_source_f,
        'sink': blocks.vector_sink_f,
        'mult': blocks.multiply_matrix_ff,
    },
    'complex': {
        'src':  blocks.vector_source_c,
        'sink': blocks.vector_sink_c,
        'mult': blocks.multiply_matrix_cc,
    },
}

class test_multiply_matrix_xx (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.multiplier = None

    def tearDown (self):
        self.tb = None
        self.multiplier = None

    def run_once(self,
            X_in,
            A,
            tpp=gr.TPP_DONT,
            A2=None,
            tags=None,
            msg_A=None,
            datatype='float',
        ):
        """ Run the test for given input-, output- and matrix values.
        Every row from X_in is considered an input signal on a port. """
        X_in = numpy.matrix(X_in)
        A_matrix = numpy.matrix(A)
        (N, M) = A_matrix.shape
        self.assertTrue(N == X_in.shape[0])
        # Calc expected
        Y_out_exp = numpy.matrix(numpy.zeros((M, X_in.shape[1])))
        self.multiplier = BLOCK_LOOKUP[datatype]['mult'](A, tpp)
        if A2 is not None:
            self.multiplier.set_A(A2)
            A = A2
            A_matrix = numpy.matrix(A)
        for i in xrange(N):
            if tags is None:
                these_tags = ()
            else:
                these_tags = (tags[i],)
            self.tb.connect(
                    BLOCK_LOOKUP[datatype]['src'](X_in[i].tolist()[0], tags=these_tags),
                    (self.multiplier, i)
            )
        sinks = []
        for i in xrange(M):
            sinks.append(BLOCK_LOOKUP[datatype]['sink']())
            self.tb.connect((self.multiplier, i), sinks[i])
        # Run and check
        self.tb.run()
        for i in xrange(X_in.shape[1]):
            Y_out_exp[:,i] = A_matrix * X_in[:,i]
        Y_out = [list(x.data()) for x in sinks]
        if tags is not None:
            self.the_tags = []
            for i in xrange(M):
                self.the_tags.append(sinks[i].tags())
        self.assertEqual(list(Y_out), Y_out_exp.tolist())


    def test_001_t (self):
        """ Simplest possible check: N==M, unit matrix """
        X_in = (
            (1, 2, 3, 4),
            (5, 6, 7, 8),
        )
        A = (
            (1, 0),
            (0, 1),
        )
        self.run_once(X_in, A)

    def test_001_t_complex (self):
        """ Simplest possible check: N==M, unit matrix """
        X_in = (
            (1, 2, 3, 4),
            (5, 6, 7, 8),
        )
        A = (
            (1, 0),
            (0, 1),
        )
        self.run_once(X_in, A, datatype='complex')

    def test_002_t (self):
        """ Switch check: N==M, flipped unit matrix """
        X_in = (
            (1, 2, 3, 4),
            (5, 6, 7, 8),
        )
        A = (
            (0, 1),
            (1, 0),
        )
        self.run_once(X_in, A)

    def test_003_t (self):
        """ Average """
        X_in = (
            (1, 1, 1, 1),
            (2, 2, 2, 2),
        )
        A = (
            (0.5, 0.5),
            (0.5, 0.5),
        )
        self.run_once(X_in, A)

    def test_004_t (self):
        """ Set """
        X_in = (
            (1, 2, 3, 4),
            (5, 6, 7, 8),
        )
        A1 = (
            (1, 0),
            (0, 1),
        )
        A2 = (
            (0, 1),
            (1, 0),
        )
        self.run_once(X_in, A1, A2=A2)

    def test_005_t (self):
        """ Tags """
        X_in = (
            (1, 2, 3, 4),
            (5, 6, 7, 8),
        )
        A = (
            (0, 1), # Flip them round
            (1, 0),
        )
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.intern("in1")
        tag1.value = pmt.PMT_T
        tag2 = gr.tag_t()
        tag2.offset = 0
        tag2.key = pmt.intern("in2")
        tag2.value = pmt.PMT_T
        self.run_once(X_in, A, tpp=gr.TPP_ONE_TO_ONE, tags=(tag1, tag2))
        self.assertTrue(pmt.equal(tag1.key, self.the_tags[0][0].key))
        self.assertTrue(pmt.equal(tag2.key, self.the_tags[1][0].key))

    #def test_006_t (self):
        #""" Message passing """
        #X_in = (
            #(1, 2, 3, 4),
            #(5, 6, 7, 8),
        #)
        #A1 = (
            #(1, 0),
            #(0, 1),
        #)
        #msg_A = (
            #(0, 1),
            #(1, 0),
        #)
        #self.run_once(X_in, A1, msg_A=msg_A)



if __name__ == '__main__':
    #gr_unittest.run(test_multiply_matrix_ff, "test_multiply_matrix_ff.xml")
    gr_unittest.run(test_multiply_matrix_xx)

