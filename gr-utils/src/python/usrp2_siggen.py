#!/usr/bin/env python
#
# Copyright 2008,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation
from gnuradio import usrp2
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math

n2s = eng_notation.num_to_str

class siggen_top_block(gr.top_block):
    def __init__(self, options):
        gr.top_block.__init__(self)

        # Create a USRP2 sink with the requested interpolation rate
        self._u = usrp2.sink_32fc(options.interface, options.mac_addr)
        self._u.set_interp(options.interp)

        # Set the Tx daughterboard gain as requested
        if options.gain is None:
            g = self._u.gain_range()
            options.gain = float(g[0]+g[1])/2
        self._u.set_gain(options.gain)

        # Tune the USRP2 FPGA and daughterboard to the requested center frequency
        # and LO offset
        if options.lo_offset is not None:
            self._u.set_lo_offset(options.lo_offset)

        tr = self._u.set_center_freq(options.tx_freq)
        if tr == None:
            sys.stderr.write('Failed to set center frequency\n')
            raise SystemExit, 1

        eth_rate = self._u.dac_rate()/self._u.interp()
        
        # Create a source for the requested waveform type
        if options.type == gr.GR_SIN_WAVE or options.type == gr.GR_CONST_WAVE:
            self._src = gr.sig_source_c(eth_rate,              # Sample rate
                                        options.type,          # Waveform type
                                        options.waveform_freq, # Waveform frequency
                                        options.amplitude,     # Waveform amplitude
                                        options.offset)        # Waveform offset
            
        elif options.type == gr.GR_GAUSSIAN or options.type == gr.GR_UNIFORM:
            self._src = gr.noise_source_c(options.type, options.amplitude)
        elif options.type == "2tone":
            self._src1 = gr.sig_source_c(eth_rate,
                                         gr.GR_SIN_WAVE,
                                         options.waveform_freq,
                                         options.amplitude,
                                         0)
            if(options.waveform2_freq is None):
                w2freq = -options.waveform_freq
            else:
                w2freq = options.waveform2_freq
            self._src2 = gr.sig_source_c(eth_rate,
                                         gr.GR_SIN_WAVE,
                                         w2freq,
                                         options.amplitude,
                                         0)
            self._src = gr.add_cc()
            self.connect(self._src1,(self._src,0))
            self.connect(self._src2,(self._src,1))
        elif options.type == "sweep":
            # rf freq is center frequency
            # waveform_freq is total swept width
            # waveform2_freq is sweep rate
            #  will sweep from (rf_freq-waveform_freq/2) to (rf_freq+waveform_freq/2)
            self._src1 = gr.sig_source_f(eth_rate,
                                         gr.GR_TRI_WAVE,
                                         options.waveform2_freq,
                                         1.0,  # options.waveform_freq,
                                         -0.5)
            self._src2 = gr.frequency_modulator_fc(options.waveform_freq*2*math.pi/eth_rate)
            self._src = gr.multiply_const_cc(options.amplitude)
            self.connect(self._src1,self._src2,self._src)
        else:
            sys.stderr.write('Unknown waveform type\n')
            raise SystemExit, 1

        if options.verbose:
            print "Network interface:", options.interface
            print "USRP2 address:", self._u.mac_addr()
            #print "Using TX d'board %s" % (self._u.tx_name(),)
            print "Tx gain:", options.gain
            print "Tx baseband frequency:", n2s(tr.baseband_freq), "Hz"
            print "Tx DDC frequency:", n2s(tr.dxc_freq), "Hz"
            print "Tx residual frequency:", n2s(tr.residual_freq), "Hz"
            print "Tx interpolation rate:", options.interp
            print "Tx GbE sample rate:", n2s(eth_rate), "samples/sec"
	    if options.type == gr.GR_SIN_WAVE:
		print "Baseband waveform type: Sine wave"
		print "Baseband waveform frequency:", n2s(options.waveform_freq), "Hz"
	    elif options.type == gr.GR_CONST_WAVE:
		print "Baseband waveform type: Constant"
	    elif options.type == gr.GR_GAUSSIAN:
		print "Baseband waveform type: Gaussian noise"
	    elif options.type == gr.GR_UNIFORM:
		print "Baseband waveform type: Uniform noise"		
			    
        # Wire the flowgraph
        self.connect(self._src, self._u)
            
def get_options():
    usage="%prog: [options]"

    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-e", "--interface", type="string", default="eth0",
                      help="use specified Ethernet interface [default=%default]")
    parser.add_option("-m", "--mac-addr", type="string", default="",
                      help="use USRP2 at specified MAC address [default=None]")  
    parser.add_option("-i", "--interp", type="int", default=16,
                      help="set fgpa decimation rate to DECIM [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set output gain to GAIN [default=%default]")
    parser.add_option("-f", "--tx-freq", type="eng_float", default=None,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="verbose output")
    parser.add_option("-w", "--waveform-freq", type="eng_float", default=0,
                      help="set waveform frequency to FREQ [default=%default]")
    parser.add_option("-x", "--waveform2-freq", type="eng_float", default=None,
                      help="set waveform frequency to FREQ [default=%default]")
    parser.add_option("-a", "--amplitude", type="eng_float", default=0.5,
                      help="set waveform amplitude to AMPLITUDE (0-1.0) [default=%default]", metavar="AMPL")
    parser.add_option("--offset", type="eng_float", default=0,
                      help="set waveform offset to OFFSET [default=%default]")
    parser.add_option("--lo-offset", type="eng_float", default=None,
                      help="set daughterboard LO offset to OFFSET [default=hw default]")
    parser.add_option("--sine", dest="type", action="store_const", const=gr.GR_SIN_WAVE,
                      help="generate a complex sinusoid [default]", default=gr.GR_SIN_WAVE)
    parser.add_option("--const", dest="type", action="store_const", const=gr.GR_CONST_WAVE, 
                      help="generate a constant output")
    parser.add_option("--gaussian", dest="type", action="store_const", const=gr.GR_GAUSSIAN,
                      help="generate Gaussian random output")
    parser.add_option("--uniform", dest="type", action="store_const", const=gr.GR_UNIFORM,
                      help="generate Uniform random output")
    parser.add_option("--2tone", dest="type", action="store_const", const="2tone",
                      help="generate Two Tone signal for IMD testing")
    parser.add_option("--sweep", dest="type", action="store_const", const="sweep",
                      help="generate a swept sine wave")
                      
    (options, args) = parser.parse_args ()
    if len(args) != 0:
        parser.print_help()
        raise SystemExit, 1
    
    if options.tx_freq is None:
        parser.print_help()
        sys.stderr.write('You must specify the frequency with -f FREQ\n');
        raise SystemExit, 1
    
    return options


if __name__ == '__main__':
    options = get_options()
    tb = siggen_top_block(options)
    
    # Attempt to enable realtime scheduling
    r = gr.enable_realtime_scheduling()
    if r == gr.RT_OK:
        realtime = True
    else:
        realtime = False
        print "Note: failed to enable realtime scheduling"

    try:
        tb.run()
    except KeyboardInterrupt:
        pass
