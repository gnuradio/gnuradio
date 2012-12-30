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
#


# Audio output with a volume control.
#
# M. Revnell 2005-Dec

from gnuradio import gr, gru
from gnuradio import blocks
from gnuradio import audio

class output( gr.hier_block2 ):
    def __init__( self, rate, device ):
        gr.hier_block2.__init__(self, "output",
                                gr.io_signature(1,1,gr.sizeof_float),
                                gr.io_signature(0,0,0))

        self.vol = blocks.multiply_const_ff( 0.1 )
        self.out = audio.sink( int(rate), device )

        self.connect( self, self.vol, self.out )

    def set( self, val ):
        self.vol.set_k( val )

