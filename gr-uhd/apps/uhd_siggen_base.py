#!/usr/bin/env python
#
# Copyright 2008,2009,2011,2012,2015 Free Software Foundation, Inc.
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
"""
Provide a base flow graph for USRP signal generators.
"""

from __future__ import print_function
import math
try:
    from uhd_app import UHDApp
except ImportError:
    from gnuradio.uhd.uhd_app import UHDApp
from gnuradio import gr, uhd, eng_notation, eng_arg
from gnuradio import analog
from gnuradio import blocks
from gnuradio.gr.pubsub import pubsub

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

n2s = eng_notation.num_to_str

WAVEFORMS = {
    analog.GR_CONST_WAVE : "Constant",
    analog.GR_SIN_WAVE   : "Complex Sinusoid",
    analog.GR_GAUSSIAN   : "Gaussian Noise",
    analog.GR_UNIFORM    : "Uniform Noise",
    "2tone"              : "Two Tone",
    "sweep"              : "Sweep",
}

class USRPSiggen(gr.top_block, pubsub, UHDApp):
    """
    GUI-unaware GNU Radio flowgraph.  This may be used either with command
    line applications or GUI applications.
    """
    def __init__(self, args):
        gr.top_block.__init__(self)
        pubsub.__init__(self)
        UHDApp.__init__(self, args=args, prefix="UHD-SIGGEN")
        self.extra_sink = None

        # Allocate some attributes
        self._src1 = None
        self._src2 = None
        self._src = None

        # Initialize device:
        self.setup_usrp(
                ctor=uhd.usrp_sink,
                args=args,
        )
        print("[UHD-SIGGEN] UHD Signal Generator")
        print("[UHD-SIGGEN] UHD Version: {ver}".format(ver=uhd.get_version_string()))
        print("[UHD-SIGGEN] Using USRP configuration:")
        print(self.get_usrp_info_string(tx_or_rx="tx"))
        self.usrp_description = self.get_usrp_info_string(tx_or_rx="tx", compact=True)

        ### Set subscribers and publishers:
        self.publish(SAMP_RATE_KEY, lambda: self.usrp.get_samp_rate())
        self.publish(DESC_KEY, lambda: self.usrp_description)
        self.publish(FREQ_RANGE_KEY, lambda: self.usrp.get_freq_range(self.channels[0]))
        self.publish(GAIN_RANGE_KEY, lambda: self.usrp.get_gain_range(self.channels[0]))
        self.publish(GAIN_KEY, lambda: self.usrp.get_gain(self.channels[0]))

        self[SAMP_RATE_KEY] = args.samp_rate
        self[TX_FREQ_KEY] = args.freq
        self[AMPLITUDE_KEY] = args.amplitude
        self[WAVEFORM_FREQ_KEY] = args.waveform_freq
        self[WAVEFORM_OFFSET_KEY] = args.offset
        self[WAVEFORM2_FREQ_KEY] = args.waveform2_freq
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
        self[TYPE_KEY] = args.type #set type last

    def set_samp_rate(self, samp_rate):
        """
        When sampling rate is updated, also update the signal sources.
        """
        self.vprint("Setting sampling rate to: {rate} Msps".format(rate=samp_rate/1e6))
        self.usrp.set_samp_rate(samp_rate)
        samp_rate = self.usrp.get_samp_rate()
        if self[TYPE_KEY] in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE):
            self._src.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
        else:
            return True # Waveform not yet set
        self.vprint("Set sample rate to: {rate} Msps".format(rate=samp_rate/1e6))
        return True

    def set_waveform_freq(self, freq):
        " Change the frequency 1 of the generated waveform "
        if self[TYPE_KEY] == analog.GR_SIN_WAVE:
            self._src.set_frequency(freq)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_frequency(freq)
        elif self[TYPE_KEY] == 'sweep':
            #there is no set sensitivity, redo fg
            self[TYPE_KEY] = self[TYPE_KEY]
        return True

    def set_waveform2_freq(self, freq):
        """
        Change the frequency 2 of the generated waveform. This only
        applies to 2-tone and sweep.
        """
        if freq is None:
            self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]
            return
        if self[TYPE_KEY] == "2tone":
            self._src2.set_frequency(freq)
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_frequency(freq)
        return True

    def set_waveform(self, waveform_type):
        """
        Select the generated waveform
        """
        self.vprint("Selecting waveform...")
        self.lock()
        self.disconnect_all()
        if waveform_type == analog.GR_SIN_WAVE or waveform_type == analog.GR_CONST_WAVE:
            self._src = analog.sig_source_c(self[SAMP_RATE_KEY],      # Sample rate
                                            waveform_type,                # Waveform waveform_type
                                            self[WAVEFORM_FREQ_KEY], # Waveform frequency
                                            self[AMPLITUDE_KEY],     # Waveform amplitude
                                            self[WAVEFORM_OFFSET_KEY])        # Waveform offset
        elif waveform_type == analog.GR_GAUSSIAN or waveform_type == analog.GR_UNIFORM:
            self._src = analog.noise_source_c(waveform_type, self[AMPLITUDE_KEY])
        elif waveform_type == "2tone":
            self._src1 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                             analog.GR_SIN_WAVE,
                                             self[WAVEFORM_FREQ_KEY],
                                             self[AMPLITUDE_KEY]/2.0,
                                             0)
            if self[WAVEFORM2_FREQ_KEY] is None:
                self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]
            self._src2 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                             analog.GR_SIN_WAVE,
                                             self[WAVEFORM2_FREQ_KEY],
                                             self[AMPLITUDE_KEY]/2.0,
                                             0)
            self._src = blocks.add_cc()
            self.connect(self._src1, (self._src, 0))
            self.connect(self._src2, (self._src, 1))
        elif waveform_type == "sweep":
            # rf freq is center frequency
            # waveform_freq is total swept width
            # waveform2_freq is sweep rate
            # will sweep from (rf_freq-waveform_freq/2) to (rf_freq+waveform_freq/2)
            if self[WAVEFORM2_FREQ_KEY] is None:
                self[WAVEFORM2_FREQ_KEY] = 0.1
            self._src1 = analog.sig_source_f(self[SAMP_RATE_KEY],
                                             analog.GR_TRI_WAVE,
                                             self[WAVEFORM2_FREQ_KEY],
                                             1.0,
                                             -0.5)
            self._src2 = analog.frequency_modulator_fc(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
            self._src = blocks.multiply_const_cc(self[AMPLITUDE_KEY])
            self.connect(self._src1, self._src2, self._src)
        else:
            raise RuntimeError("[UHD-SIGGEN] Unknown waveform waveform_type")
        for chan in xrange(len(self.channels)):
            self.connect(self._src, (self.usrp, chan))
        if self.extra_sink is not None:
            self.connect(self._src, self.extra_sink)
        self.unlock()
        self.vprint("Set baseband modulation to:", WAVEFORMS[waveform_type])
        if waveform_type == analog.GR_SIN_WAVE:
            self.vprint("Modulation frequency: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),))
            self.vprint("Initial phase:", self[WAVEFORM_OFFSET_KEY])
        elif waveform_type == "2tone":
            self.vprint("Tone 1: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),))
            self.vprint("Tone 2: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),))
        elif waveform_type == "sweep":
            self.vprint("Sweeping across %sHz to %sHz" % (n2s(-self[WAVEFORM_FREQ_KEY]/2.0), n2s(self[WAVEFORM_FREQ_KEY]/2.0)))
            self.vprint("Sweep rate: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),))
        self.vprint("TX amplitude:", self[AMPLITUDE_KEY])

    def set_amplitude(self, amplitude):
        """
        amplitude subscriber
        """
        if amplitude < 0.0 or amplitude > 1.0:
            self.vprint("Amplitude out of range:", amplitude)
            return False
        if self[TYPE_KEY] in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE, analog.GR_GAUSSIAN, analog.GR_UNIFORM):
            self._src.set_amplitude(amplitude)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_amplitude(amplitude/2.0)
            self._src2.set_amplitude(amplitude/2.0)
        elif self[TYPE_KEY] == "sweep":
            self._src.set_k(amplitude)
        else:
            return True # Waveform not yet set
        self.vprint("Set amplitude to:", amplitude)
        return True


def setup_argparser():
    """
    Create argument parser for signal generator.
    """
    parser = UHDApp.setup_argparser(
            description="USRP Signal Generator.",
            tx_or_rx="Tx",
    )
    group = parser.add_argument_group('Siggen Arguments')
    group.add_argument("-x", "--waveform-freq", type=eng_arg.eng_float, default=0.0,
                      help="Set baseband waveform frequency to FREQ")
    group.add_argument("-y", "--waveform2-freq", type=eng_arg.eng_float, default=0.0,
                      help="Set 2nd waveform frequency to FREQ")
    group.add_argument("--sine", dest="type", action="store_const", const=analog.GR_SIN_WAVE,
                      help="Generate a carrier modulated by a complex sine wave",
                      default=analog.GR_SIN_WAVE)
    group.add_argument("--const", dest="type", action="store_const", const=analog.GR_CONST_WAVE,
                      help="Generate a constant carrier")
    group.add_argument("--offset", type=eng_arg.eng_float, default=0,
                      help="Set waveform phase offset to OFFSET", metavar="OFFSET")
    group.add_argument("--gaussian", dest="type", action="store_const", const=analog.GR_GAUSSIAN,
                      help="Generate Gaussian random output")
    group.add_argument("--uniform", dest="type", action="store_const", const=analog.GR_UNIFORM,
                      help="Generate Uniform random output")
    group.add_argument("--2tone", dest="type", action="store_const", const="2tone",
                      help="Generate Two Tone signal for IMD testing")
    group.add_argument("--sweep", dest="type", action="store_const", const="sweep",
                      help="Generate a swept sine wave")
    return parser

def main():
    " Go, go, go! "
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Note: failed to enable realtime scheduling, continuing")
    # Grab command line args and create top block
    try:
        parser = setup_argparser()
        args = parser.parse_args()
        tb = USRPSiggen(args)
    except RuntimeError as ex:
        print(ex)
        exit(1)
    tb.start()
    raw_input('[UHD-SIGGEN] Press Enter to quit:\n')
    tb.stop()
    tb.wait()

if __name__ == "__main__":
    main()
