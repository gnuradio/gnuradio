#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 Caliola Engineering, LLC.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, blocks


class matrix_interleaver(gr.hier_block2):
    """
    Block interleaver writes inputs into conceptual rows of the matrix
    and reads outputs by conceptual columns of the matrix.
    In deinterleaver mode it writes inputs into conceptual columns
    and reads outputs by conceptual rows.
    """

    def __init__(self, itemsize, rows=1, cols=1, deint=False):
        gr.hier_block2.__init__(
            self, "Matrix Interleaver",
            gr.io_signature(1, 1, itemsize),
            gr.io_signature(1, 1, itemsize),
        )

        self.itemsize = itemsize
        self.set_rowsandcols(rows, cols, deint)

    def set_rowsandcols(self, rows, cols, deint):
        self.disconnect_all()

        self.passthrough = None
        self.interleaver = None
        self.deinterleaver = None

        ##################################################
        # Parameters
        ##################################################
        self.rows = rows
        self.cols = cols
        self.deint = deint

        ##################################################
        # Blocks
        ##################################################
        # short circuit for unitary rows / columns
        if rows == 1 or cols == 1:
            self.passthrough = blocks.copy(self.itemsize)
            self.connect((self, 0), (self.passthrough, 0), (self, 0))
            return

        self.deinterleaver = blocks.deinterleave(
            self.itemsize, 1 if deint else cols)
        self.interleaver = blocks.interleave(
            self.itemsize, cols if deint else 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self, 0), (self.deinterleaver, 0))
        for n in range(rows):
            self.connect((self.deinterleaver, n), (self.interleaver, n))
        self.connect((self.interleaver, 0), (self, 0))
