# Audio output with a volume control.
#
# M. Revnell 2005-Dec

from gnuradio import gr, gru
from gnuradio import audio

class output( gr.hier_block ):
    def __init__( self, fg, rate ):
        self.out = audio.sink( rate )
        self.vol = gr.multiply_const_ff( 0.1 )
        fg.connect( self.vol, self.out )
        gr.hier_block.__init__(self, fg, self.vol, None )

    def set( self, val ):
        self.vol.set_k( val )
        
