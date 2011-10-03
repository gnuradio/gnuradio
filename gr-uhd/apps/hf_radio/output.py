# Audio output with a volume control.
#
# M. Revnell 2005-Dec

from gnuradio import gr, gru
from gnuradio import audio

class output( gr.hier_block2 ):
    def __init__( self, rate, device ):
        gr.hier_block2.__init__(self, "output",
                                gr.io_signature(1,1,gr.sizeof_float),
                                gr.io_signature(0,0,0))
        
        self.vol = gr.multiply_const_ff( 0.1 )
        self.out = audio.sink( int(rate), device )

        self.connect( self, self.vol, self.out )

    def set( self, val ):
        self.vol.set_k( val )
        
