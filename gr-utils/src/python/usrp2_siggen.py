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

from gnuradio import gr, eng_notation, usrp2
from gnuradio.eng_option import eng_option
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
class top_block(gr.top_block):
    def __init__(self, options, args):
        gr.top_block.__init__(self)
        self._verbose = options.verbose

        self._interp = 0 
        self._gain = 0
        self._freq = None       # Indicates frequency hasn't been successfully set yet
        self._bb_freq = 0
        self._ddc_freq = 0
        self._amplitude = 0
        self._type = None       # Indicates waveform flowgraph not created yet
        self._offset = options.offset

        self.set_usrp2(options.interface, options.mac_addr)
        self.set_interp(options.interp)
        self.set_gain(options.gain)
        self.set_freq(options.tx_freq, options.lo_offset)
        self.set_amplitude(options.amplitude)

        self.set_waveform_freq(options.waveform_freq)
        #self.set_waveform2_freq(options.waveform2_freq)
        self._waveform2_freq = options.waveform2_freq
        self.set_waveform(options.type)

    def set_usrp2(self, interface, mac_addr):
        self._u = usrp2.sink_32fc(interface, mac_addr)
        self._dac_rate = self._u.dac_rate()
        if self._verbose:
            print "Network interface:", interface
            print "Network address:", self._u.mac_addr()
            print "Daughterboard ID:", hex(self._u.daughterboard_id())

    def set_interp(self, interp):
        if interp < 4 or interp > 512: # FIXME get from flowgraph
            if self._verbose: print "Interpolation rate out of range:", interp
            return False

        if not self._u.set_interp(interp):
            raise RuntimeError("Failed to set interpolation rate %i" % (interp,))

        self._interp = interp
        self._eth_rate = self._dac_rate/self._interp
        if self._verbose:
            print "USRP2 interpolation rate:", self._interp
            print "USRP2 IF bandwidth: %sHz" % (n2s(self._eth_rate),)

        if (self._type == gr.GR_SIN_WAVE or 
            self._type == gr.GR_CONST_WAVE):
            self._src.set_sampling_freq(self._eth_rate)
        elif self._type == "2tone":
            self._src1.set_sampling_freq(self._eth_rate)
            self._src1.set_sampling_freq(self._eth_rate)
        elif self._type == "sweep":
            self._src1.set_sampling_freq(self._eth_rate)
            self._src1.set_sampling_freq(self._waveform_freq*2*math.pi/self._eth_rate)
        else:
            return True # Waveform not yet set
        
        if self._verbose: print "Set interpolation rate to:", interp
        return True

    def set_gain(self, gain):
        if gain is None:
            g = self._u.gain_range()
            gain = float(g[0]+g[1])/2
            if self._verbose:
                print "Using auto-calculated mid-point TX gain"
        self._u.set_gain(gain)
        self._gain = gain
        if self._verbose:
            print "Set TX gain to:", self._gain

    def set_freq(self, target_freq, lo_offset=None):
        if lo_offset is not None:
            self._lo_offset = lo_offset
            self._u.set_lo_offset(self._lo_offset)
            if self._verbose:
                print "Set LO offset frequency to: %sHz" % (n2s(lo_offset),)

        if target_freq is None:
            f = self._u.freq_range()
            target_freq = float(f[0]+f[1])/2.0
            if self._verbose:
                print "Using auto-calculated mid-point frequency"

        tr = self._u.set_center_freq(target_freq)
        fs = "%sHz" % (n2s(target_freq),)
        if tr is not None:
            self._freq = target_freq

        else:
            return True # Waveform not yet set
        
        if self._verbose: print "Set amplitude to:", amplitude
        return True

    def set_gain(self, gain):
        if gain is None:
            g = self._u.gain_range()
            gain = float(g[0]+g[1])/2
            if self._verbose:
                print "Using auto-calculated mid-point TX gain"
        self._u.set_gain(gain)
        self._gain = gain
        if self._verbose:
            print "Set TX gain to:", self._gain

    def set_freq(self, target_freq, lo_offset=None):
        if lo_offset is not None:
            self._lo_offset = lo_offset
            self._u.set_lo_offset(self._lo_offset)
            if self._verbose:
                print "Set LO offset frequency to: %sHz" % (n2s(lo_offset),)

        if target_freq is None:
            f = self._u.freq_range()
            target_freq = float(f[0]+f[1])/2.0
            if self._verbose:
                print "Using auto-calculated mid-point frequency"

        tr = self._u.set_center_freq(target_freq)
        fs = "%sHz" % (n2s(target_freq),)
        if tr is not None:
            self._freq = target_freq
            self._ddc_freq = tr.dxc_freq
            self._bb_freq = tr.baseband_freq
            if self._verbose:
                print "Set center frequency to", fs
                print "Tx baseband frequency: %sHz" % (n2s(tr.baseband_freq),)
                print "Tx DDC frequency: %sHz" % (n2s(tr.dxc_freq),)
                print "Tx residual frequency: %sHz" % (n2s(tr.residual_freq),)
                
        return tr

    def set_waveform_freq(self, freq):
        self._waveform_freq = freq
        if self._type == gr.GR_SIN_WAVE:
            self._src.set_frequency(freq)
        elif self._type == "2tone" or self._type == "sweep":
            self._src1.set_frequency(freq)
        return True

    def set_waveform2_freq(self, freq):
        self._waveform2_freq = freq
        if self._type == "2tone":
            self._src2.set_frequency(freq)
        elif self._type == "sweep":
            self._src1.set_frequency(freq)
        return True

    def set_waveform(self, type):
        self.lock()
        self.disconnect_all()

        if type == gr.GR_SIN_WAVE or type == gr.GR_CONST_WAVE:
            self._src = gr.sig_source_c(self._eth_rate,      # Sample rate
                                        type,                # Waveform type
                                        self._waveform_freq, # Waveform frequency
                                        self._amplitude,     # Waveform amplitude
                                        self._offset)        # Waveform offset
        elif type == gr.GR_GAUSSIAN or type == gr.GR_UNIFORM:
            self._src = gr.noise_source_c(type, self._amplitude)
        elif type == "2tone":
            self._src1 = gr.sig_source_c(self._eth_rate,
                                         gr.GR_SIN_WAVE,
                                         self._waveform_freq,
                                         self._amplitude/2.0,
                                         0)
            if(self._waveform2_freq is None):
                self._waveform2_freq = -self._waveform_freq

            self._src2 = gr.sig_source_c(self._eth_rate,
                                         gr.GR_SIN_WAVE,
                                         self._waveform2_freq,
                                         self._amplitude/2.0,
                                         0)
            self._src = gr.add_cc()
            self.connect(self._src1,(self._src,0))
            self.connect(self._src2,(self._src,1))
        elif type == "sweep":
            # rf freq is center frequency
            # waveform_freq is total swept width
            # waveform2_freq is sweep rate
            # will sweep from (rf_freq-waveform_freq/2) to (rf_freq+waveform_freq/2)
            if self._waveform2_freq is None:
                self._waveform2_freq = 0.1

            self._src1 = gr.sig_source_f(self._eth_rate,
                                         gr.GR_TRI_WAVE,
                                         self._waveform2_freq,
                                         1.0,
                                         -0.5)
            self._src2 = gr.frequency_modulator_fc(self._waveform_freq*2*math.pi/self._eth_rate)
            self._src = gr.multiply_const_cc(self._amplitude)
            self.connect(self._src1,self._src2,self._src)
        else:
            raise RuntimeError("Unknown waveform type")

        self.connect(self._src, self._u)
        self._type = type
        self.unlock()

        if self._verbose:
            print "Set baseband modulation to:", waveforms[self._type]
            if type == gr.GR_SIN_WAVE:
                print "Modulation frequency: %sHz" % (n2s(self._waveform_freq),)
                print "Initial phase:", self._offset
            elif type == "2tone":
                print "Tone 1: %sHz" % (n2s(self._waveform_freq),)
                print "Tone 2: %sHz" % (n2s(self._waveform2_freq),)
            elif type == "sweep":
                print "Sweeping across %sHz to %sHz" % (n2s(-self._waveform_freq/2.0),n2s(self._waveform_freq/2.0))
                print "Sweep rate: %sHz" % (n2s(self._waveform2_freq),)
            print "TX amplitude:", self._amplitude


    def set_amplitude(self, amplitude):
        if amplitude < 0.0 or amplitude > 1.0:
            if self._verbose: print "Amplitude out of range:", amplitude
            return False

        self._amplitude = amplitude

        if (self._type == gr.GR_SIN_WAVE or 
            self._type == gr.GR_CONST_WAVE or
            self._type == gr.GR_GAUSSIAN or
            self._type == gr.GR_UNIFORM):
            self._src.set_amplitude(amplitude)
        elif self._type == "2tone":
            self._src1.set_amplitude(amplitude/2.0)
            self._src2.set_amplitude(amplitude/2.0)
        elif self._type == "sweep":
            self._src.set_k(amplitude)
        else:
            return True # Waveform not yet set
        
        if self._verbose: print "Set amplitude to:", amplitude
        return True


    # Property getters

    #def interface(self):
    #    return self._u.ifc_name()

    def mac_addr(self):
        return self._u.mac_addr()

    def ifc_name(self):
        return self._u.ifc_name()

    def daughterboard_id(self):
        return self._u.daughterboard_id()

    def interp_rate(self):
        return self._interp

    def eth_rate(self):
        return self._eth_rate

    def freq(self):
        return self._freq

    def freq_range(self):
        fr = self._u.freq_range()
        if self._u.daughterboard_id() == 0x0000:
            fr = (-50e6, 50e6) # DEBUG
        return fr

    def ddc_freq(self):
        return self._ddc_freq

    def baseband_freq(self):
        return self._bb_freq

    def amplitude(self):
        return self._amplitude

    def waveform_type(self):
        return self._type

    def waveform_freq(self):
        return self._waveform_freq

    def waveform2_freq(self):
        if self._waveform2_freq is None:
            return -self._waveform_freq
        else:
            return self._waveform2_freq

def get_options():
    usage="%prog: [options]"

    parser = OptionParser(option_class=eng_option, usage=usage)

    parser.add_option("-e", "--interface", type="string", default="eth0",
                      help="Use specified Ethernet interface [default=%default]")
    parser.add_option("-m", "--mac-addr", type="string", default="",
                      help="Use USRP2 at specified MAC address [default=None]")  
    parser.add_option("-i", "--interp", type="int", default=16, metavar="INTERP",
                      help="Set FPGA interpolation rate of INTERP [default=%default]")
    parser.add_option("-f", "--tx-freq", type="eng_float", default=None,
                      help="Set carrier frequency to FREQ [default=mid-point]", metavar="FREQ")
    parser.add_option("--lo-offset", type="eng_float", default=None,
                      help="set daughterboard LO offset to OFFSET [default=hw default]")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="Set TX gain to GAIN [default=mid-point]")
    parser.add_option("-w", "--waveform-freq", type="eng_float", default=0,
                      help="Set baseband waveform frequency to FREQ [default=%default]")
    parser.add_option("-x", "--waveform2-freq", type="eng_float", default=None,
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
    parser.add_option("-a", "--amplitude", type="eng_float", default=0.1,
                      help="Set output amplitude to AMPL (0.0-1.0) [default=%default]", metavar="AMPL")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="Use verbose console output [default=%default]")

    (options, args) = parser.parse_args()

    return (options, args)

# If this script is executed, the following runs. If it is imported, the below does not run.
if __name__ == "__main__":
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Note: failed to enable realtime scheduling, continuing"
    
    # Grab command line options and create top block
    try:
        (options, args) = get_options()
        tb = top_block(options, args)

    except RuntimeError, e:
        print e
        sys.exit(1)

    # Run it
    try:
        tb.run()

    except KeyboardInterrupt:
        pass
