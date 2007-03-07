# Basic USRP setup and control.
# It's only ever been tried with a basic rx daughter card.
#
# Imagine that the gnuradio boilerplate is here.
#
# M. Revnell 2005-Dec
 
from gnuradio import gr, gru, optfir
from gnuradio import usrp
from usrpm import usrp_dbid
import math

# Put special knowlege of usrp here.

class input:
    def __init__( self, decim ):
        self.freq = -2.5e6
        self.src = usrp.source_c( )
        self.subdev = usrp.pick_subdev( self.src,
                                        (usrp_dbid.BASIC_RX,
                                         usrp_dbid.TV_RX))

        print self.subdev

        self.subdevice = usrp.selected_subdev( self.src,
                                               self.subdev )

        self.mux = usrp.determine_rx_mux_value( self.src,
                                                self.subdev )
        self.decim = decim

        self.adc_rate = self.src.adc_rate()
        self.usrp_rate = self.adc_rate / self.decim
        self.src.set_decim_rate( self.decim )
        self.src.set_mux( self.mux )
        usrp.tune( self.src, 0, self.subdevice, self.freq )

    def set_freq( self, x ):
        r = usrp.tune( self.src, 0, self.subdevice, -x )
        if r:
            self.freq = -x
