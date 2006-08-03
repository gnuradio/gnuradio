#!/usr/bin/env python

from gnuradio import gr, gru, usrp, optfir, audio, eng_notation, blks
from gnuradio.eng_option import eng_option
from optparse import OptionParser

"""
This example application demonstrates receiving and demodulating 
different types of signals using the USRP. 

A receive chain is built up of the following signal processing
blocks:

USRP  - Daughter board source generating complex baseband signal.
CHAN  - Low pass filter to select channel bandwidth
RFSQL - RF squelch zeroing output when input power below threshold
AGC   - Automatic gain control leveling signal at [-1.0, +1.0]
DEMOD - Demodulation block appropriate to selected signal type.
        This converts the complex baseband to real audio frequencies,
	and applies an appropriate low pass decimating filter.
CTCSS - Optional tone squelch zeroing output when tone is not present.
RSAMP - Resampler block to convert audio sample rate to user specified
        sound card output rate.
AUDIO - Audio sink for playing final output to speakers.

The following are required command line parameters:

-f FREQ		USRP receive frequency
-m MOD		Modulation type, select from AM, FM, or WFM

The following are optional command line parameters:

-R SUBDEV       Daughter board specification, defaults to first found
-c FREQ         Calibration offset.  Gets added to receive frequency.
                Defaults to 0.0 Hz.
-g GAIN         Daughterboard gain setting. Defaults to mid-range.
-o RATE         Sound card output rate. Defaults to 32000. Useful if
                your sound card only accepts particular sample rates.
-r RFSQL	RF squelch in db. Defaults to -50.0.
-p FREQ		CTCSS frequency.  Opens squelch when tone is present.

Once the program is running, ctrl-break (Ctrl-C) stops operation.

Please see fm_demod.py and am_demod.py for details of the demodulation
blocks.
"""

# (usrp_decim, channel_decim, audio_decim, channel_pass, channel_stop, demod)
demod_params = {
		'AM'  : (250, 16, 1,  5000,   8000, blks.demod_10k0a3e_cf),
		'FM'  : (250,  8, 4,  8000,   9000, blks.demod_20k0f3e_cf),
		'WFM' : (250,  1, 8, 90000, 100000, blks.demod_200kf3e_cf)
	       }

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

	(usrp_decim, channel_decim, audio_decim, 
	 channel_pass, channel_stop, demod) = demod_params[options.modulation]

        USRP = usrp_source_c(self, 		    # Flow graph
			    options.rx_subdev_spec, # Daugherboard spec
	                    usrp_decim,     	    # IF decimation ratio
			    options.gain, 	    # Receiver gain
			    options.calibration)    # Frequency offset
	USRP.tune(options.frequency)

	if_rate = USRP.rate()
        channel_rate = if_rate // channel_decim
	audio_rate = channel_rate // audio_decim

	CHAN_taps = optfir.low_pass(1.0,         # Filter gain
				   if_rate, 	 # Sample rate
				   channel_pass, # One sided modulation bandwidth
	                           channel_stop, # One sided channel bandwidth
				   0.1, 	 # Passband ripple
				   60) 		 # Stopband attenuation

	CHAN = gr.freq_xlating_fir_filter_ccf(channel_decim, # Decimation rate
	                                      CHAN_taps,     # Filter taps
					      0.0, 	     # Offset frequency
					      if_rate)	     # Sample rate

	RFSQL = gr.pwr_squelch_cc(options.rf_squelch,    # Power threshold
	                          125.0/channel_rate, 	 # Time constant
				  channel_rate/20,       # 50ms rise/fall
				  False)		 # Zero, not gate output

	AGC = gr.agc_cc(1.0/channel_rate,  # Time constant
			1.0,     	   # Reference power 
			1.0,               # Initial gain
			1.0)		   # Maximum gain

	DEMOD = demod(self, channel_rate, audio_decim)

	# From RF to audio
        self.connect(USRP, CHAN, RFSQL, AGC, DEMOD)

	# Optionally add CTCSS and RSAMP if needed
	tail = DEMOD
	if options.ctcss != None and options.ctcss > 60.0:
	    CTCSS = gr.ctcss_squelch_ff(audio_rate,    # Sample rate
				        options.ctcss) # Squelch tone
	    self.connect(DEMOD, CTCSS)
	    tail = CTCSS

	if options.output_rate != audio_rate:
	    out_lcm = gru.lcm(audio_rate, options.output_rate)
	    out_interp = int(out_lcm // audio_rate)
	    out_decim = int(out_lcm // options.output_rate)
	    RSAMP = blks.rational_resampler_fff(self, out_interp, out_decim)
	    self.connect(tail, RSAMP)
	    tail = RSAMP 

	# Send to default audio output
        AUDIO = audio.sink(options.output_rate, "")
	self.connect(tail, AUDIO)
	
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
    parser.add_option("-m", "--modulation", type="choice", choices=('AM','FM','WFM'),
                      help="set modulation type (AM,FM)", metavar="TYPE")
    parser.add_option("-o", "--output-rate", type="int", default=32000,
                      help="set audio output rate to RATE", metavar="RATE")
    parser.add_option("-r", "--rf-squelch", type="eng_float", default=-50.0,
                      help="set RF squelch to dB", metavar="dB")
    parser.add_option("-p", "--ctcss", type="float",
		      help="set CTCSS squelch to FREQ", metavar="FREQ")
    (options, args) = parser.parse_args()

    if options.frequency < 1e6:
	options.frequency *= 1e6
	
    fg = app_flow_graph(options, args)
    try:
        fg.run()
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    main()
