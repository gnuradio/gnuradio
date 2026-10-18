#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Quantizer
# Generated: Thu Aug  1 11:09:51 2013
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes


class quantizer(gr.hier_block2):

    def __init__(self, bits=16):
        gr.hier_block2.__init__(
            self, "Quantizer",
            gr.io_signature(1, 1, gr.sizeof_float * 1),
            gr.io_signature(1, 1, gr.sizeof_float * 1),
        )

        ##################################################
        # Parameters
        ##################################################
        self._check_bits(bits)
        self.bits = bits

        ##################################################
        # Blocks
        ##################################################
        # saturate to the signed range of the given bit depth
        self.rail = analog.rail_ff(-pow(2.0, bits - 1.0), pow(2.0, bits - 1.0) - 1)
        self.blocks_short_to_float_0 = blocks.short_to_float(1, 1)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vff(
            (1.0 / pow(2.0, bits - 1.0), ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff(
            (pow(2, bits - 1.0), ))
        self.blocks_float_to_short_0 = blocks.float_to_short(1, 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self, 0))
        self.connect((self.blocks_short_to_float_0, 0),
                     (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.blocks_float_to_short_0, 0),
                     (self.blocks_short_to_float_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.rail, 0))
        self.connect((self.rail, 0), (self.blocks_float_to_short_0, 0))
        self.connect((self, 0), (self.blocks_multiply_const_vxx_0, 0))

    @staticmethod
    def _check_bits(bits):
        if not 1 <= bits <= 16:
            raise ValueError("quantizer: bits must be in [1, 16]")

    def get_bits(self):
        return self.bits

    def set_bits(self, bits):
        self._check_bits(bits)
        self.bits = bits
        self.rail.set_lo(-pow(2.0, self.bits - 1.0))
        self.rail.set_hi(pow(2.0, self.bits - 1.0) - 1)
        self.blocks_multiply_const_vxx_0_0.set_k(
            (1.0 / pow(2.0, self.bits - 1.0), ))
        self.blocks_multiply_const_vxx_0.set_k((pow(2, self.bits - 1.0), ))
