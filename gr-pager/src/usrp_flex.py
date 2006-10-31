#!/usr/bin/env python

from gnuradio import gr, gru, usrp, optfir, eng_notation, blks, pager
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import time, os, sys
from string import split, join

"""
This example application demonstrates receiving and demodulating the
FLEX pager protocol.

A receive chain is built up of the following signal processing
blocks:

USRP  - Daughter board source generating complex baseband signal.
CHAN  - Low pass filter to select channel bandwidth
AGC   - Automatic gain control leveling signal at [-1.0, +1.0]
FLEX  - FLEX pager protocol decoder

The following are required command line parameters:

-f FREQ		USRP receive frequency

The following are optional command line parameters:

-R SUBDEV   Daughter board specification, defaults to first found
-c FREQ     Calibration offset.  Gets added to receive frequency.
            Defaults to 0.0 Hz.
-g GAIN     Daughterboard gain setting. Defaults to mid-range.
-r RFSQL	RF squelch in db. Defaults to -50.0.

Once the program is running, ctrl-break (Ctrl-C) stops operation.
"""

class usrp_source_c(gr.hier_block):
    """
    Create a USRP source object supplying complex floats.
    
    Selects user supplied subdevice or chooses first available one.

    Calibration value is the offset from the tuned frequency to 
    the actual frequency.       
    """
    def __init__(self, fg, subdev_spec, decim, gain=None, calibration=0.0):
        self._decim = decim
        self._src = usrp.source_c()
        if subdev_spec is None:
            subdev_spec = usrp.pick_rx_subdevice(self._src)
        self._subdev = usrp.selected_subdev(self._src, subdev_spec)
        self._src.set_mux(usrp.determine_rx_mux_value(self._src, subdev_spec))
        self._src.set_decim_rate(self._decim)

        # If no gain specified, set to midrange
        if gain is None:
            g = self._subdev.gain_range()
            gain = (g[0]+g[1])/2.0

        self._subdev.set_gain(gain)
        self._cal = calibration

        gr.hier_block.__init__(self, fg, self._src, self._src)

    def tune(self, freq):
        result = usrp.tune(self._src, 0, self._subdev, freq+self._cal)
        # TODO: deal with residual

    def rate(self):
        return self._src.adc_rate()/self._decim

class app_flow_graph(gr.flow_graph):
    def __init__(self, options, args, queue):
        gr.flow_graph.__init__(self)
        self.options = options
        self.args = args

        USRP = usrp_source_c(self,          # Flow graph
                    options.rx_subdev_spec, # Daugherboard spec
	            256,                    # IF decimation ratio gets 250K if_rate
                    options.gain,           # Receiver gain
                    options.calibration)    # Frequency offset
        USRP.tune(options.frequency)

        if_rate = USRP.rate()
        channel_rate = 25000                
        channel_decim = int(if_rate / channel_rate)
	
        CHAN_taps = optfir.low_pass(1.0,          # Filter gain
                                    if_rate,      # Sample rate
			            8000,         # One sided modulation bandwidth
	                            10000,        # One sided channel bandwidth
				    0.1,	  # Passband ripple
				    60) 	  # Stopband attenuation
	
        CHAN = gr.freq_xlating_fir_filter_ccf(channel_decim, # Decimation rate
                                              CHAN_taps,     # Filter taps
                                              0.0,           # Offset frequency
                                              if_rate)       # Sample rate

        AGC = gr.agc_cc(1.0/channel_rate,  # Time constant
                        1.0,               # Reference power 
                        1.0,               # Initial gain
                        1.0)               # Maximum gain
	
        FLEX = pager.flex_demod(self, 25000, queue)

        self.connect(USRP, CHAN, AGC, FLEX.INPUT)
	
def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float",
                      help="set receive frequency to Hz", metavar="Hz")
    parser.add_option("-R", "--rx-subdev-spec", type="subdev",
                      help="select USRP Rx side A or B", metavar="SUBDEV")
    parser.add_option("-c",   "--calibration", type="eng_float", default=0.0,
                      help="set frequency offset to Hz", metavar="Hz")
    parser.add_option("-g", "--gain", type="int", default=None,
                      help="set RF gain", metavar="dB")
    (options, args) = parser.parse_args()

    if options.frequency < 1e6:
	options.frequency *= 1e6
	
    queue = gr.msg_queue()

    fg = app_flow_graph(options, args, queue)
    try:
        fg.start()
	while 1:
	    msg = queue.delete_head() # Blocking read
	    fields = split(msg.to_string(), chr(128))
	    print join(fields, '|')

    except KeyboardInterrupt:
        fg.stop()

if __name__ == "__main__":
    main()
