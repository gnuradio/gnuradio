#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Caliola Engineering, LLC.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, blocks

class matrix_interleaver(gr.hier_block2):
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

        self.deinterleaver = blocks.deinterleave(self.itemsize, 1 if deint else cols)
        self.interleaver = blocks.interleave(self.itemsize, cols if deint else 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self, 0), (self.deinterleaver, 0))
        for n in range(rows):
            self.connect((self.deinterleaver, n), (self.interleaver, n))
        self.connect((self.interleaver, 0), (self, 0))
