# post detection agc processing
#
# Imagine that the usual gnuradio copyright stuff is right here.
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

from gnuradio import gr, gru

class agc( gr.hier_block ):
    def __init__( self, fg ):
        self.split = gr.multiply_const_ff( 1 )
        self.sqr   = gr.multiply_ff( )
        self.int0  = gr.iir_filter_ffd( [.004, 0], [0, .999] )
        self.offs  = gr.add_const_ff( -30 )
        self.gain  = gr.multiply_const_ff( 70 )
        self.log   = gr.nlog10_ff( 10, 1 )
        self.agc   = gr.divide_ff( )

        fg.connect(   self.split,        ( self.agc,   0 ) )
        fg.connect(   self.split,        ( self.sqr,   0 ) )
        fg.connect(   self.split,        ( self.sqr,   1 ) )
        fg.connect(   self.sqr,            self.int0       )
        fg.connect(   self.int0,           self.log        )
        fg.connect(   self.log,            self.offs       )
        fg.connect(   self.offs,           self.gain       )
        fg.connect(   self.gain,         ( self.agc,   1 ) )

        gr.hier_block.__init__( self, fg, self.split, self.agc )
