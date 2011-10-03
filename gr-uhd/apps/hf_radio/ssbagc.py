# Copyright 2011 Free Software Foundation, Inc.
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
 

# post detection agc processing
#
# This agc strategy is copied more or less verbatim from
# weaver_isb_am1_usrp3.py by cswiger.
#
# Thanks.
#
# Then modified in a variety of ways.
#
# There doesn't appear to be a way to hook multiple blocks to the
# input port when building a hier block like this. Thus the
# split below.
#
# Basic operation.
# Power is estimated by squaring the input.
# Low pass filter using a 1 pole iir.
# The time constant can be tweaked by changing the taps.
# Currently there is no implementation to change this while operating
#   a potentially useful addition.
# The log block turns this into dB
# gain adjusts the agc authority.
#
# M. Revnell 2006-Jan

from gnuradio import gr

class agc( gr.hier_block2 ):
    def __init__( self ):
        gr.hier_block2.__init__(self, "agc",
                                gr.io_signature(1,1,gr.sizeof_float),
                                gr.io_signature(1,1,gr.sizeof_float))

        self.split = gr.multiply_const_ff( 1 )
        self.sqr   = gr.multiply_ff( )
        self.int0  = gr.iir_filter_ffd( [.004, 0], [0, .999] )
        self.offs  = gr.add_const_ff( -30 )
        self.gain  = gr.multiply_const_ff( 70 )
        self.log   = gr.nlog10_ff( 10, 1 )
        self.agc   = gr.divide_ff( )

        self.connect(self,       self.split)
        self.connect(self.split, (self.agc, 0))
        self.connect(self.split, (self.sqr, 0))
        self.connect(self.split, (self.sqr, 1))
        self.connect(self.sqr,    self.int0)
        self.connect(self.int0,   self.log)
        self.connect(self.log,    self.offs)
        self.connect(self.offs,   self.gain)
        self.connect(self.gain,  (self.agc, 1))
        self.connect(self.agc,    self)
