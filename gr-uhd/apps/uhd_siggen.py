#!/usr/bin/env python
#
# Copyright 2008,2009,2011,2012 Free Software Foundation, Inc.
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
GAIN_KEY = 'gain'
TX_FREQ_KEY = 'tx_freq'
DSP_FREQ_KEY = 'dsp_freq'
RF_FREQ_KEY = 'rf_freq'
AMPLITUDE_KEY = 'amplitude'
AMPL_RANGE_KEY = 'ampl_range'
WAVEFORM_FREQ_KEY = 'waveform_freq'
WAVEFORM_OFFSET_KEY = 'waveform_offset'
WAVEFORM2_FREQ_KEY = 'waveform2_freq'
FREQ_RANGE_KEY = 'freq_range'
GAIN_RANGE_KEY = 'gain_range'
TYPE_KEY = 'type'

def setter(ps, key, val): ps[key] = val

from gnuradio import gr, gru, uhd, eng_notation
from gnuradio.gr.pubsub import pubsub
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
class top_block(gr.top_block, pubsub):
    def __init__(self, options, args):
        gr.top_block.__init__(self)
        pubsub.__init__(self)
        self._verbose = options.verbose

        #initialize values from options
        self._setup_usrpx(options)
        self[SAMP_RATE_KEY] = options.samp_rate
        self[TX_FREQ_KEY] = options.tx_freq
        self[AMPLITUDE_KEY] = options.amplitude
        self[WAVEFORM_FREQ_KEY] = options.waveform_freq
        self[WAVEFORM_OFFSET_KEY] = options.offset
        self[WAVEFORM2_FREQ_KEY] = options.waveform2_freq
        self[DSP_FREQ_KEY] = 0
        self[RF_FREQ_KEY] = 0

        #subscribe set methods
        self.subscribe(SAMP_RATE_KEY, self.set_samp_rate)
        self.subscribe(GAIN_KEY, self.set_gain)
        self.subscribe(TX_FREQ_KEY, self.set_freq)
        self.subscribe(AMPLITUDE_KEY, self.set_amplitude)
        self.subscribe(WAVEFORM_FREQ_KEY, self.set_waveform_freq)
        self.subscribe(WAVEFORM2_FREQ_KEY, self.set_waveform2_freq)
        self.subscribe(TYPE_KEY, self.set_waveform)

        #force update on pubsub keys
        for key in (SAMP_RATE_KEY, GAIN_KEY, TX_FREQ_KEY, 
                    AMPLITUDE_KEY, WAVEFORM_FREQ_KEY,
                    WAVEFORM_OFFSET_KEY, WAVEFORM2_FREQ_KEY):
            self[key] = self[key]
        self[TYPE_KEY] = options.type #set type last

    def _setup_usrpx(self, options):
        self._u = uhd.usrp_sink(device_addr=options.args, stream_args=uhd.stream_args('fc32'))
        self._u.set_samp_rate(options.samp_rate)

        # Set the subdevice spec
        if(options.spec):
            self._u.set_subdev_spec(options.spec, 0)

        # Set the gain on the usrp from options
        if(options.gain):
            self._u.set_gain(options.gain)

        # Set the antenna
        if(options.antenna):
            self._u.set_antenna(options.antenna, 0)

        # Setup USRP Configuration value
        try:
            usrp_info = self._u.get_usrp_info()
            mboard_id = usrp_info.get("mboard_id").split(" ")[0]
            mboard_serial = usrp_info.get("mboard_serial")
            if mboard_serial == "":
                mboard_serial = "no serial"
            dboard_id = usrp_info.get("tx_id").split(" ")[0].split(",")[0]
            dboard_serial = usrp_info.get("tx_serial")
            if dboard_serial == "":
                dboard_serial = "no serial"
            subdev = self._u.get_subdev_spec()
            antenna = self._u.get_antenna()
            
            desc_key_str = "Motherboard: %s [%s]\n" % (mboard_id, mboard_serial)
            desc_key_str += "Daughterboard: %s [%s]\n" % (dboard_id, dboard_serial)
            desc_key_str += "Subdev: %s\n" % subdev
            desc_key_str += "Antenna: %s" % antenna
        except:
            desc_key_str = "USRP configuration output not implemented in this version"

        self.publish(DESC_KEY, lambda: desc_key_str)
        self.publish(FREQ_RANGE_KEY, self._u.get_freq_range)
        self.publish(GAIN_RANGE_KEY, self._u.get_gain_range)
        self.publish(GAIN_KEY, self._u.get_gain)

        print "UHD Signal Generator"
        print "Version: %s" % uhd.get_version_string()
        print "\nUsing USRP configuration:"
        print desc_key_str + "\n"

        # Direct asynchronous notifications to callback function
        if options.show_async_msg:
            self.async_msgq = gr.msg_queue(0)
            self.async_src = uhd.amsg_source("", self.async_msgq)
            self.async_rcv = gru.msgq_runner(self.async_msgq, self.async_callback)

    def async_callback(self, msg):
        md = self.async_src.msg_to_async_metadata_t(msg)
        print "Channel: %i Time: %f Event: %i" % (md.channel, md.time_spec.get_real_secs(), md.event_code)

    def _set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP
        @param ampl the amplitude or None for automatic
        """
        ampl_range = self[AMPL_RANGE_KEY]
        if ampl is None:
            ampl = (ampl_range[1] - ampl_range[0])*0.15 + ampl_range[0]
        self[AMPLITUDE_KEY] = max(ampl_range[0], min(ampl, ampl_range[1]))

    def set_samp_rate(self, sr):
        self._u.set_samp_rate(sr)
        sr = self._u.get_samp_rate()

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
        
        if self._verbose:
            print "Set sample rate to:", sr

        return True

    def set_gain(self, gain):
        if gain is None:
            g = self[GAIN_RANGE_KEY]
            gain = float(g.start()+g.stop())/2
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
            target_freq = float(f.start()+f.stop())/2.0
            if self._verbose:
                print "Using auto-calculated mid-point frequency"
            self[TX_FREQ_KEY] = target_freq
            return

        tr = self._u.set_center_freq(target_freq)
        fs = "%sHz" % (n2s(target_freq),)
        if tr is not None:
            self._freq = target_freq
            self[DSP_FREQ_KEY] = tr.actual_dsp_freq
            self[RF_FREQ_KEY] = tr.actual_rf_freq
            if self._verbose:
                print "Set center frequency to", self._u.get_center_freq()
                print "Tx RF frequency:  %sHz" % (n2s(tr.actual_rf_freq),)
                print "Tx DSP frequency: %sHz" % (n2s(tr.actual_dsp_freq),)
        elif self._verbose:
            print "Failed to set freq." 
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
            if self._verbose:
                print "Amplitude out of range:", amplitude
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
        
        if self._verbose:
            print "Set amplitude to:", amplitude
        return True

def get_options():
    usage="%prog: [options]"

    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-a", "--args", type="string", default="",
                      help="UHD device address args , [default=%default]")
    parser.add_option("", "--spec", type="string", default=None,
                      help="Subdevice of UHD device where appropriate")
    parser.add_option("-A", "--antenna", type="string", default=None,
                      help="select Rx Antenna where appropriate")
    parser.add_option("-s", "--samp-rate", type="eng_float", default=1e6,
                      help="set sample rate (bandwidth) [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set gain in dB (default is midpoint)")
    parser.add_option("-f", "--tx-freq", type="eng_float", default=None,
                      help="Set carrier frequency to FREQ [default=mid-point]",
                      metavar="FREQ")
    parser.add_option("-x", "--waveform-freq", type="eng_float", default=0,
                      help="Set baseband waveform frequency to FREQ [default=%default]")
    parser.add_option("-y", "--waveform2-freq", type="eng_float", default=None,
                      help="Set 2nd waveform frequency to FREQ [default=%default]")
    parser.add_option("--sine", dest="type", action="store_const", const=gr.GR_SIN_WAVE,
                      help="Generate a carrier modulated by a complex sine wave",
                      default=gr.GR_SIN_WAVE)
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
    parser.add_option("", "--amplitude", type="eng_float", default=0.15,
                      help="Set output amplitude to AMPL (0.0-1.0) [default=%default]",
                      metavar="AMPL")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="Use verbose console output [default=%default]")
    parser.add_option("", "--show-async-msg", action="store_true", default=False,
                      help="Show asynchronous message notifications from UHD [default=%default]")

    (options, args) = parser.parse_args()

    return (options, args)

# If this script is executed, the following runs. If it is imported,
# the below does not run.
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
if __name__ == "__main__":
    main()
