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

DESC_KEY = 'desc'
SAMP_RATE_KEY = 'samp_rate'
LINK_RATE_KEY = 'link_rate'
DAC_RATE_KEY = 'dac_rate'
INTERP_KEY = 'interp'
GAIN_KEY = 'gain'
TX_FREQ_KEY = 'tx_freq'
DDC_FREQ_KEY = 'ddc_freq'
BB_FREQ_KEY = 'bb_freq'
AMPLITUDE_KEY = 'amplitude'
AMPL_RANGE_KEY = 'ampl_range'
WAVEFORM_FREQ_KEY = 'waveform_freq'
WAVEFORM_OFFSET_KEY = 'waveform_offset'
WAVEFORM2_FREQ_KEY = 'waveform2_freq'
FREQ_RANGE_KEY = 'freq_range'
GAIN_RANGE_KEY = 'gain_range'
TYPE_KEY = 'type'

def setter(ps, key, val): ps[key] = val

from gnuradio import gr, eng_notation
from gnuradio.gr.pubsub import pubsub
from gnuradio.eng_option import eng_option
from gnuradio import usrp_options
from optparse import OptionParser
import sys
import math

n2s = eng_notation.num_to_str

waveforms = { gr.GR_SIN_WAVE   : "Complex Sinusoid",
              gr.GR_CONST_WAVE : "Constant",
              gr.GR_GAUSSIAN   : "Gaussian Noise",
              gr.GR_UNIFORM    : "Uniform Noise",
              "2tone"          : "Two Tone",
              "sweep"          : "Sweep" }

#
# GUI-unaware GNU Radio flowgraph.  This may be used either with command
# line applications or GUI applications.
#
class top_block(gr.top_block, pubsub):
    def __init__(self, options, args):
        gr.top_block.__init__(self)
        pubsub.__init__(self)
        self._verbose = options.verbose
        #initialize values from options
        self._setup_usrpx(options)
        self.subscribe(INTERP_KEY, lambda i: setter(self, SAMP_RATE_KEY, self[DAC_RATE_KEY]/i))
        self.subscribe(SAMP_RATE_KEY, lambda e: setter(self, LINK_RATE_KEY, e*32))
        self[INTERP_KEY] = options.interp or 16
        self[TX_FREQ_KEY] = options.tx_freq
        self[AMPLITUDE_KEY] = options.amplitude
        self[WAVEFORM_FREQ_KEY] = options.waveform_freq
        self[WAVEFORM_OFFSET_KEY] = options.offset
        self[WAVEFORM2_FREQ_KEY] = options.waveform2_freq
        self[BB_FREQ_KEY] = 0
        self[DDC_FREQ_KEY] = 0
        #subscribe set methods
        self.subscribe(INTERP_KEY, self.set_interp)
        self.subscribe(GAIN_KEY, self.set_gain)
        self.subscribe(TX_FREQ_KEY, self.set_freq)
        self.subscribe(AMPLITUDE_KEY, self.set_amplitude)
        self.subscribe(WAVEFORM_FREQ_KEY, self.set_waveform_freq)
        self.subscribe(WAVEFORM2_FREQ_KEY, self.set_waveform2_freq)
        self.subscribe(TYPE_KEY, self.set_waveform)
        #force update on pubsub keys
        for key in (INTERP_KEY, GAIN_KEY, TX_FREQ_KEY,
            AMPLITUDE_KEY, WAVEFORM_FREQ_KEY, WAVEFORM_OFFSET_KEY, WAVEFORM2_FREQ_KEY):
            self[key] = self[key]
        self[TYPE_KEY] = options.type #set type last

    def _setup_usrpx(self, options):
        self._u = usrp_options.create_usrp_sink(options)
        self.publish(DESC_KEY, lambda: str(self._u))
        self.publish(DAC_RATE_KEY, self._u.dac_rate)
        self.publish(FREQ_RANGE_KEY, self._u.freq_range)
        self.publish(GAIN_RANGE_KEY, self._u.gain_range)
        self.publish(GAIN_KEY, self._u.gain)
        if self._verbose: print str(self._u)

    def _set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP
        @param ampl the amplitude or None for automatic
        """
        ampl_range = self[AMPL_RANGE_KEY]
        if ampl is None: ampl = (ampl_range[1] - ampl_range[0])*0.15 + ampl_range[0]
        self[AMPLITUDE_KEY] = max(ampl_range[0], min(ampl, ampl_range[1]))

    def set_interp(self, interp):
        if not self._u.set_interp(interp):
            raise RuntimeError("Failed to set interpolation rate %i" % (interp,))

        if self._verbose:
            print "USRP interpolation rate:", interp
            print "USRP IF bandwidth: %sHz" % (n2s(self[SAMP_RATE_KEY]),)

        if self[TYPE_KEY] in (gr.GR_SIN_WAVE, gr.GR_CONST_WAVE):
            self._src.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
        else:
            return True # Waveform not yet set
        
        if self._verbose: print "Set interpolation rate to:", interp
        return True

    def set_gain(self, gain):
        if gain is None:
            g = self[GAIN_RANGE_KEY]
            gain = float(g[0]+g[1])/2
            if self._verbose:
                print "Using auto-calculated mid-point TX gain"
            self[GAIN_KEY] = gain
            return
        self._u.set_gain(gain)
        if self._verbose:
            print "Set TX gain to:", gain

    def set_freq(self, target_freq):

        if target_freq is None:
            f = self[FREQ_RANGE_KEY]
            target_freq = float(f[0]+f[1])/2.0
            if self._verbose:
                print "Using auto-calculated mid-point frequency"
            self[TX_FREQ_KEY] = target_freq
            return

        tr = self._u.set_center_freq(target_freq)
        fs = "%sHz" % (n2s(target_freq),)
        if tr is not None:
            self._freq = target_freq
            self[DDC_FREQ_KEY] = tr.dxc_freq
            self[BB_FREQ_KEY] = tr.baseband_freq
            if self._verbose:
                print "Set center frequency to", fs
                print "Tx baseband frequency: %sHz" % (n2s(tr.baseband_freq),)
                print "Tx DDC frequency: %sHz" % (n2s(tr.dxc_freq),)
                print "Tx residual frequency: %sHz" % (n2s(tr.residual_freq),)
        elif self._verbose: print "Failed to set freq." 
        return tr

    def set_waveform_freq(self, freq):
        if self[TYPE_KEY] == gr.GR_SIN_WAVE:
            self._src.set_frequency(freq)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_frequency(freq)
        elif self[TYPE_KEY] == 'sweep':
            #there is no set sensitivity, redo fg
            self[TYPE_KEY] = self[TYPE_KEY]
        return True

    def set_waveform2_freq(self, freq):
        if freq is None:
            self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]
            return
        if self[TYPE_KEY] == "2tone":
            self._src2.set_frequency(freq)
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_frequency(freq)
        return True

    def set_waveform(self, type):
        self.lock()
        self.disconnect_all()
        if type == gr.GR_SIN_WAVE or type == gr.GR_CONST_WAVE:
            self._src = gr.sig_source_c(self[SAMP_RATE_KEY],      # Sample rate
                                        type,                # Waveform type
                                        self[WAVEFORM_FREQ_KEY], # Waveform frequency
                                        self[AMPLITUDE_KEY],     # Waveform amplitude
                                        self[WAVEFORM_OFFSET_KEY])        # Waveform offset
        elif type == gr.GR_GAUSSIAN or type == gr.GR_UNIFORM:
            self._src = gr.noise_source_c(type, self[AMPLITUDE_KEY])
        elif type == "2tone":
            self._src1 = gr.sig_source_c(self[SAMP_RATE_KEY],
                                         gr.GR_SIN_WAVE,
                                         self[WAVEFORM_FREQ_KEY],
                                         self[AMPLITUDE_KEY]/2.0,
                                         0)
            if(self[WAVEFORM2_FREQ_KEY] is None):
                self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]

            self._src2 = gr.sig_source_c(self[SAMP_RATE_KEY],
                                         gr.GR_SIN_WAVE,
                                         self[WAVEFORM2_FREQ_KEY],
                                         self[AMPLITUDE_KEY]/2.0,
                                         0)
            self._src = gr.add_cc()
            self.connect(self._src1,(self._src,0))
            self.connect(self._src2,(self._src,1))
        elif type == "sweep":
            # rf freq is center frequency
            # waveform_freq is total swept width
            # waveform2_freq is sweep rate
            # will sweep from (rf_freq-waveform_freq/2) to (rf_freq+waveform_freq/2)
            if self[WAVEFORM2_FREQ_KEY] is None:
                self[WAVEFORM2_FREQ_KEY] = 0.1

            self._src1 = gr.sig_source_f(self[SAMP_RATE_KEY],
                                         gr.GR_TRI_WAVE,
                                         self[WAVEFORM2_FREQ_KEY],
                                         1.0,
                                         -0.5)
            self._src2 = gr.frequency_modulator_fc(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
            self._src = gr.multiply_const_cc(self[AMPLITUDE_KEY])
            self.connect(self._src1,self._src2,self._src)
        else:
            raise RuntimeError("Unknown waveform type")

        self.connect(self._src, self._u)
        self.unlock()

        if self._verbose:
            print "Set baseband modulation to:", waveforms[type]
            if type == gr.GR_SIN_WAVE:
                print "Modulation frequency: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),)
                print "Initial phase:", self[WAVEFORM_OFFSET_KEY]
            elif type == "2tone":
                print "Tone 1: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),)
                print "Tone 2: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),)
            elif type == "sweep":
                print "Sweeping across %sHz to %sHz" % (n2s(-self[WAVEFORM_FREQ_KEY]/2.0),n2s(self[WAVEFORM_FREQ_KEY]/2.0))
                print "Sweep rate: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),)
            print "TX amplitude:", self[AMPLITUDE_KEY]


    def set_amplitude(self, amplitude):
        if amplitude < 0.0 or amplitude > 1.0:
            if self._verbose: print "Amplitude out of range:", amplitude
            return False

        if self[TYPE_KEY] in (gr.GR_SIN_WAVE, gr.GR_CONST_WAVE, gr.GR_GAUSSIAN, gr.GR_UNIFORM):
            self._src.set_amplitude(amplitude)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_amplitude(amplitude/2.0)
            self._src2.set_amplitude(amplitude/2.0)
        elif self[TYPE_KEY] == "sweep":
            self._src.set_k(amplitude)
        else:
            return True # Waveform not yet set
        
        if self._verbose: print "Set amplitude to:", amplitude
        return True

def get_options():
    usage="%prog: [options]"

    parser = OptionParser(option_class=eng_option, usage=usage)
    usrp_options.add_tx_options(parser)
    parser.add_option("-f", "--tx-freq", type="eng_float", default=None,
                      help="Set carrier frequency to FREQ [default=mid-point]", metavar="FREQ")
    parser.add_option("-x", "--waveform-freq", type="eng_float", default=0,
                      help="Set baseband waveform frequency to FREQ [default=%default]")
    parser.add_option("-y", "--waveform2-freq", type="eng_float", default=None,
                      help="Set 2nd waveform frequency to FREQ [default=%default]")
    parser.add_option("--sine", dest="type", action="store_const", const=gr.GR_SIN_WAVE,
                      help="Generate a carrier modulated by a complex sine wave", default=gr.GR_SIN_WAVE)
    parser.add_option("--const", dest="type", action="store_const", const=gr.GR_CONST_WAVE, 
                      help="Generate a constant carrier")
    parser.add_option("--offset", type="eng_float", default=0,
                      help="Set waveform phase offset to OFFSET [default=%default]")
    parser.add_option("--gaussian", dest="type", action="store_const", const=gr.GR_GAUSSIAN,
                      help="Generate Gaussian random output")
    parser.add_option("--uniform", dest="type", action="store_const", const=gr.GR_UNIFORM,
                      help="Generate Uniform random output")
    parser.add_option("--2tone", dest="type", action="store_const", const="2tone",
                      help="Generate Two Tone signal for IMD testing")
    parser.add_option("--sweep", dest="type", action="store_const", const="sweep",
                      help="Generate a swept sine wave")
    parser.add_option("-A", "--amplitude", type="eng_float", default=0.15,
                      help="Set output amplitude to AMPL (0.0-1.0) [default=%default]", metavar="AMPL")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="Use verbose console output [default=%default]")

    (options, args) = parser.parse_args()

    return (options, args)

# If this script is executed, the following runs. If it is imported, the below does not run.
def main():
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Note: failed to enable realtime scheduling, continuing"
    
    # Grab command line options and create top block
    try:
        (options, args) = get_options()
        tb = top_block(options, args)

    except RuntimeError, e:
        print e
        sys.exit(1)

    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()

# Make sure to create the top block (tb) within a function:
# That code in main will allow tb to go out of scope on return,
# which will call the decontructor on usrp and stop transmit.
# Whats odd is that grc works fine with tb in the __main__,
# perhaps its because the try/except clauses around tb.
if __name__ == "__main__": main()
