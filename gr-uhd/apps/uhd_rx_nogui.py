#!/usr/bin/env python
#
# Copyright 2006,2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, uhd, optfir, audio, blks2
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

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

# (device_rate, channel_rate, audio_rate, channel_pass, channel_stop, demod)
demod_params = {
		'AM'  : (256e3,  16e3, 16e3,  5000,   8000, blks2.demod_10k0a3e_cf),
		'FM'  : (256e3,  32e3,  8e3,  8000,   9000, blks2.demod_20k0f3e_cf),
		'WFM' : (320e3, 320e3, 32e3, 80000, 115000, blks2.demod_200kf3e_cf)
	       }

class uhd_src(gr.hier_block2):
    """
    Create a UHD source object supplying complex floats.
    
    Selects user supplied subdevice or chooses first available one.

    Calibration value is the offset from the tuned frequency to 
    the actual frequency.       
    """
    def __init__(self, args, spec, antenna, samp_rate, gain=None, calibration=0.0):
	gr.hier_block2.__init__(self, "uhd_src",
				gr.io_signature(0, 0, 0),                    # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._src = uhd.usrp_source(device_addr=args, stream_args=uhd.stream_args('fc32'))

        # Set the subdevice spec
        if(spec):
            self._src.set_subdev_spec(spec, 0)
            
        # Set the antenna
        if(antenna):
            self._src.set_antenna(antenna, 0)
        
        self._src.set_samp_rate(samp_rate)
	dev_rate = self._src.get_samp_rate()
        self._samp_rate = samp_rate
        
        # Resampler to get to exactly samp_rate no matter what dev_rate is
        self._rrate = samp_rate / dev_rate
        self._resamp = blks2.pfb_arb_resampler_ccf(self._rrate)
        
	# If no gain specified, set to midrange
        gain_range = self._src.get_gain_range()
	if gain is None:
	    gain = (gain_range.start()+gain_range.stop())/2.0
            print "Using gain: ", gain
        self._src.set_gain(gain)

        self._cal = calibration
	self.connect(self._src, self._resamp, self)

    def tune(self, freq):
        r = self._src.set_center_freq(freq+self._cal, 0)

    def rate(self):
        return self._samp_rate
        
class app_top_block(gr.top_block):
    def __init__(self, options):
	gr.top_block.__init__(self)
	self.options = options

	(dev_rate, channel_rate, audio_rate,
	 channel_pass, channel_stop, demod) = demod_params[options.modulation]

        DEV = uhd_src(options.args,             # UHD device address
                      options.spec,             # device subdev spec
                      options.antenna,          # device antenna
                      dev_rate,         	# device sample rate
                      options.gain, 	    	# Receiver gain
                      options.calibration)      # Frequency offset
	DEV.tune(options.frequency)

	if_rate = DEV.rate()
        channel_decim = int(if_rate // channel_rate)
	audio_decim = int(channel_rate // audio_rate)

	CHAN_taps = optfir.low_pass(1.0,          # Filter gain
                                    if_rate, 	  # Sample rate
                                    channel_pass, # One sided modulation bandwidth
                                    channel_stop, # One sided channel bandwidth
                                    0.1, 	  # Passband ripple
                                    60) 	  # Stopband attenuation

	CHAN = gr.freq_xlating_fir_filter_ccf(channel_decim, # Decimation rate
	                                      CHAN_taps,     # Filter taps
					      0.0, 	     # Offset frequency
					      if_rate)	     # Sample rate

	RFSQL = gr.pwr_squelch_cc(options.rf_squelch,    # Power threshold
	                          125.0/channel_rate, 	 # Time constant
				  int(channel_rate/20),  # 50ms rise/fall
				  False)		 # Zero, not gate output

	AGC = gr.agc_cc(1.0/channel_rate,  # Time constant
			1.0,     	   # Reference power 
			1.0,               # Initial gain
			1.0)		   # Maximum gain

	DEMOD = demod(channel_rate, audio_decim)

	# From RF to audio
        #self.connect(DEV, CHAN, RFSQL, AGC, DEMOD)
        self.connect(DEV, CHAN, DEMOD)

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
	    RSAMP = blks2.rational_resampler_fff(out_interp, out_decim)
	    self.connect(tail, RSAMP)
	    tail = RSAMP 

	# Send to audio output device
        AUDIO = audio.sink(int(options.output_rate),
                           options.audio_output)
	self.connect(tail, AUDIO)
	
def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-a", "--args", type="string", default="",
                      help="UHD device address args , [default=%default]")
    parser.add_option("", "--spec", type="string", default=None,
                      help="Subdevice of UHD device where appropriate")
    parser.add_option("-A", "--antenna", type="string", default=None,
                      help="select Rx Antenna where appropriate [default=%default]")
    parser.add_option("-f", "--frequency", type="eng_float",
                      default=None, metavar="Hz",
                      help="set receive frequency to Hz [default=%default]")
    parser.add_option("-c",   "--calibration", type="eng_float",
                      default=0.0, metavar="Hz",
                      help="set frequency offset to Hz [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float",
                      metavar="dB", default=None,
                      help="set RF gain [default is midpoint]")
    parser.add_option("-m", "--modulation", type="choice", choices=('AM','FM','WFM'),
                      metavar="TYPE", default=None,
                      help="set modulation type (AM,FM,WFM) [default=%default]")
    parser.add_option("-o", "--output-rate", type="eng_float",
                      default=32000, metavar="RATE",
                      help="set audio output rate to RATE [default=%default]")
    parser.add_option("-r", "--rf-squelch", type="eng_float",
                      default=-50.0, metavar="dB",
                      help="set RF squelch to dB [default=%default]")
    parser.add_option("-p", "--ctcss", type="float",
                      default=None, metavar="FREQ",
		      help="set CTCSS squelch to FREQ [default=%default]")
    parser.add_option("-O", "--audio-output", type="string", default="",
                      help="pcm device name.  E.g., hw:0,0 or surround51 or /dev/dsp")
    (options, args) = parser.parse_args()

    if options.frequency is None:
	sys.stderr.write("Must supply receive frequency with -f.\n")
	sys.exit(1)

    if options.modulation is None:
        sys.stderr.write("Must supply a modulation type (AM, FM, WFM).\n")
        sys.exit(1)
        
    tb = app_top_block(options)
    try:
        tb.run()
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    main()
