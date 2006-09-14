#!/usr/bin/env python

from gnuradio import gr, gru, usrp, optfir, eng_notation, blks
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import time, os, sys

from flex_demod import flex_demod

"""
This example application demonstrates receiving and demodulating the
FLEX pager protocol.

A receive chain is built up of the following signal processing
blocks:

USRP  - Daughter board source generating complex baseband signal.
CHAN  - Low pass filter to select channel bandwidth
RFSQL - RF squelch zeroing output when input power below threshold
AGC   - Automatic gain control leveling signal at [-1.0, +1.0]
FLEX  - FLEX pager protocol decoder
SINK  - File sink for decoded output 

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

def make_filename(dir, freq):
    t = time.strftime('%Y%m%d-%H%M%S')
    f = 'r%s-%s.dat' % (t, eng_notation.num_to_str(freq))
    return os.path.join(dir, f)

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
    def __init__(self, options, args):
        gr.flow_graph.__init__(self)
        self.options = options
        self.args = args

        USRP = usrp_source_c(self,          # Flow graph
                    options.rx_subdev_spec, # Daugherboard spec
	                250,                    # IF decimation ratio gets 256K if_rate
                    options.gain,           # Receiver gain
                    options.calibration)    # Frequency offset
        USRP.tune(options.frequency)

        if_rate = USRP.rate()
        channel_rate = 32000                # Oversampled by 10 or 20
        channel_decim = if_rate // channel_rate
	
        CHAN_taps = optfir.low_pass(1.0,          # Filter gain
                                    if_rate,      # Sample rate
			                        8000,         # One sided modulation bandwidth
	                                10000,        # One sided channel bandwidth
				                    0.1, 	      # Passband ripple
				                    60) 		  # Stopband attenuation
	
        CHAN = gr.freq_xlating_fir_filter_ccf(channel_decim, # Decimation rate
                                              CHAN_taps,     # Filter taps
                                              0.0,           # Offset frequency
                                              if_rate)       # Sample rate

        RFSQL = gr.pwr_squelch_cc(options.rf_squelch, # Power threshold
                                  125.0/channel_rate, # Time constant
				                  channel_rate/20,    # 50ms rise/fall
                                  False)              # Zero, not gate output

        AGC = gr.agc_cc(1.0/channel_rate,  # Time constant
                        1.0,               # Reference power 
                        1.0,               # Initial gain
                        1.0)               # Maximum gain
	
        FLEX = flex_demod(self, 32000)      

        SINK = gr.file_sink(4, options.filename)

        self.connect(USRP, CHAN)
	self.connect(CHAN, RFSQL)
	self.connect(RFSQL, AGC)
	self.connect(AGC, FLEX)
	self.connect(FLEX, SINK)
	
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
    parser.add_option("-r", "--rf-squelch", type="eng_float", default=-50.0,
                      help="set RF squelch to dB", metavar="dB")
    parser.add_option("-F", "--filename", default=None)
    parser.add_option("-D", "--dir", default=None)
    (options, args) = parser.parse_args()

    if options.frequency < 1e6:
	options.frequency *= 1e6
	
    if options.filename is None and options.dir is None:
        sys.stderr.write('Must specify either -F FILENAME or -D DIR\n')
        parser.print_help()
        sys.exit(1)

    if options.filename is None:
        options.filename = make_filename(options.dir, options.frequency)

    fg = app_flow_graph(options, args)
    try:
        fg.run()
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    main()
