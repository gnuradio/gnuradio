#!/usr/bin/env python3
#
# Copyright 2008,2009,2011,2012,2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
Provide a base flow graph for USRP signal generators.
"""

import math
import sys
try:
    from uhd_app import UHDApp
except ImportError:
    from gnuradio.uhd.uhd_app import UHDApp
from gnuradio import gr, uhd, eng_notation, eng_arg
from gnuradio import analog
from gnuradio import blocks
from gnuradio.gr.pubsub import pubsub

# PyLint can't reliably detect C++ exports in modules, so let's disable that
# pylint: disable=no-member

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
TYPE_KEY = 'type'

WAVEFORMS = {
    analog.GR_CONST_WAVE: "Constant",
    analog.GR_SIN_WAVE: "Complex Sinusoid",
    analog.GR_GAUSSIAN: "Gaussian Noise",
    analog.GR_UNIFORM: "Uniform Noise",
    "2tone": "Two Tone",
    "sweep": "Sweep",
}


class USRPSiggen(gr.top_block, pubsub, UHDApp):
    """
    GUI-unaware GNU Radio flowgraph.  This may be used either with command
    line applications or GUI applications.
    """
    MIN_AMP_POWER_MODE = .001

    def __init__(self, args):
        gr.top_block.__init__(self)
        pubsub.__init__(self)
        if not 0.0 <= args.amplitude <= 1.0:
            raise ValueError(
                "Invalid value for amplitude: {}. Must be in [0.0, 1.0]"
                .format(args.amplitude))
        # If the power argument is given, we need to turn that into a power
        # *reference* level. This is a bit of a hack because we're assuming
        # knowledge of UHDApp (i.e. we're leaking abstractions). But it's simple
        # and harmless enough.
        if args.power:
            if args.amplitude < self.MIN_AMP_POWER_MODE:
                raise RuntimeError(
                    "[ERROR] Invalid amplitude: In power mode, amplitude must be "
                    "larger than {}!".format(self.MIN_AMP_POWER_MODE))
            args.power -= 20 * math.log10(args.amplitude)
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
        print(
            "[UHD-SIGGEN] UHD Version: {ver}".format(ver=uhd.get_version_string()))
        print("[UHD-SIGGEN] Using USRP configuration:")
        print(self.get_usrp_info_string(tx_or_rx="tx"))
        self.usrp_description = self.get_usrp_info_string(
            tx_or_rx="tx", compact=True)

        # Set subscribers and publishers:
        self.publish(SAMP_RATE_KEY, lambda: self.usrp.get_samp_rate())
        self.publish(DESC_KEY, lambda: self.usrp_description)
        self.publish(FREQ_RANGE_KEY,
                     lambda: self.usrp.get_freq_range(self.channels[0]))
        self.publish(GAIN_KEY, lambda: self.get_gain_or_power())

        self[SAMP_RATE_KEY] = args.samp_rate
        self[TX_FREQ_KEY] = args.freq
        self[AMPLITUDE_KEY] = args.amplitude
        self[WAVEFORM_FREQ_KEY] = args.waveform_freq
        self[WAVEFORM_OFFSET_KEY] = args.offset
        self[WAVEFORM2_FREQ_KEY] = args.waveform2_freq
        self[DSP_FREQ_KEY] = 0
        self[RF_FREQ_KEY] = 0

        # subscribe set methods
        self.subscribe(SAMP_RATE_KEY, self.set_samp_rate)
        self.subscribe(GAIN_KEY, self.set_gain_or_power)
        self.subscribe(TX_FREQ_KEY, self.set_freq)
        self.subscribe(AMPLITUDE_KEY, self.set_amplitude)
        self.subscribe(WAVEFORM_FREQ_KEY, self.set_waveform_freq)
        self.subscribe(WAVEFORM2_FREQ_KEY, self.set_waveform2_freq)
        self.subscribe(TYPE_KEY, self.set_waveform)
        self.subscribe(RF_FREQ_KEY, self.update_gain_range)

        # force update on pubsub keys
        for key in (SAMP_RATE_KEY, GAIN_KEY, TX_FREQ_KEY,
                    AMPLITUDE_KEY, WAVEFORM_FREQ_KEY,
                    WAVEFORM_OFFSET_KEY, WAVEFORM2_FREQ_KEY):
            self[key] = self[key]
        self[TYPE_KEY] = args.type  # set type last

    def set_samp_rate(self, samp_rate):
        """
        When sampling rate is updated, also update the signal sources.
        """
        self.vprint("Setting sampling rate to: {rate} Msps".format(
            rate=samp_rate / 1e6))
        self.usrp.set_samp_rate(samp_rate)
        samp_rate = self.usrp.get_samp_rate()
        if self[TYPE_KEY] in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE):
            self._src.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(
                self[WAVEFORM_FREQ_KEY] * 2 * math.pi / self[SAMP_RATE_KEY])
        else:
            return True  # Waveform not yet set
        self.vprint("Set sample rate to: {rate} Msps".format(
            rate=samp_rate / 1e6))
        return True

    def set_waveform_freq(self, freq):
        " Change the frequency 1 of the generated waveform "
        if self[TYPE_KEY] == analog.GR_SIN_WAVE:
            self._src.set_frequency(freq)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_frequency(freq)
        elif self[TYPE_KEY] == 'sweep':
            # there is no set sensitivity, redo fg
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

    def set_waveform(self, waveform_type):
        """
        Select the generated waveform
        """
        self.vprint("Selecting waveform...")
        self.lock()
        self.disconnect_all()
        if waveform_type in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE):
            self._src = analog.sig_source_c(self[SAMP_RATE_KEY],       # Sample rate
                                            waveform_type,             # Waveform waveform_type
                                            # Waveform frequency
                                            self[WAVEFORM_FREQ_KEY],
                                            # Waveform amplitude
                                            self[AMPLITUDE_KEY],
                                            self[WAVEFORM_OFFSET_KEY])  # Waveform offset
        elif waveform_type in (analog.GR_GAUSSIAN, analog.GR_UNIFORM):
            self._src = analog.noise_source_c(
                waveform_type, self[AMPLITUDE_KEY])
        elif waveform_type == "2tone":
            self._src1 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                             analog.GR_SIN_WAVE,
                                             self[WAVEFORM_FREQ_KEY],
                                             self[AMPLITUDE_KEY] / 2.0,
                                             0)
            if self[WAVEFORM2_FREQ_KEY] is None:
                self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]
            self._src2 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                             analog.GR_SIN_WAVE,
                                             self[WAVEFORM2_FREQ_KEY],
                                             self[AMPLITUDE_KEY] / 2.0,
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
            self._src2 = analog.frequency_modulator_fc(
                self[WAVEFORM_FREQ_KEY] * 2 * math.pi / self[SAMP_RATE_KEY])
            self._src = blocks.multiply_const_cc(self[AMPLITUDE_KEY])
            self.connect(self._src1, self._src2, self._src)
        else:
            raise RuntimeError("[UHD-SIGGEN] Unknown waveform waveform_type")
        for chan in range(len(self.channels)):
            self.connect(self._src, (self.usrp, chan))
        if self.extra_sink is not None:
            self.connect(self._src, self.extra_sink)
        self.unlock()
        self.vprint("Set baseband modulation to:", WAVEFORMS[waveform_type])
        n2s = eng_notation.num_to_str
        if waveform_type == analog.GR_SIN_WAVE:
            self.vprint("Modulation frequency: %sHz" %
                        (n2s(self[WAVEFORM_FREQ_KEY]),))
            self.vprint("Initial phase:", self[WAVEFORM_OFFSET_KEY])
        elif waveform_type == "2tone":
            self.vprint("Tone 1: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),))
            self.vprint("Tone 2: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),))
        elif waveform_type == "sweep":
            self.vprint("Sweeping across {} Hz to {} Hz".format(
                n2s(-self[WAVEFORM_FREQ_KEY] / 2.0), n2s(self[WAVEFORM_FREQ_KEY] / 2.0)))
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
            self._src1.set_amplitude(amplitude / 2.0)
            self._src2.set_amplitude(amplitude / 2.0)
        elif self[TYPE_KEY] == "sweep":
            self._src.set_k(amplitude)
        else:
            return True  # Waveform not yet set
        self.vprint("Set amplitude to:", amplitude)
        self.update_gain_range()
        return True

    def get_gain_or_power(self):
        """
        Depending on gain type, return either a power level or the current gain
        """
        if self.gain_type == self.GAIN_TYPE_GAIN:
            return self.usrp.get_gain(self.channels[0])
        return self.usrp.get_power_reference(self.channels[0]) \
            + 20 * math.log10(self[AMPLITUDE_KEY])

    def set_gain_or_power(self, gain_or_power):
        """
        Call this if a gain or power value changed, but you're not sure which it
        is.

        If it's a power, we subtract the signal offset to generate a reference
        power.
        """
        if self.gain_type == self.GAIN_TYPE_POWER:
            self.set_power_reference(
                gain_or_power - 20 * math.log10(self[AMPLITUDE_KEY]))
        else:
            self.set_gain(gain_or_power)

    def update_gain_range(self):
        """
        Update self.gain_range.
        """
        if self.gain_type == self.GAIN_TYPE_POWER:
            if self[AMPLITUDE_KEY] < self.MIN_AMP_POWER_MODE:
                raise RuntimeError(
                    "[ERROR] Invalid amplitude: In power mode, amplitude must be "
                    "larger than {}!".format(self.MIN_AMP_POWER_MODE))
            power_range = self.usrp.get_power_range(self.channels[0])
            ampl_offset = 20 * math.log10(self[AMPLITUDE_KEY])
            self.gain_range = uhd.meta_range(
                math.floor(power_range.start() + ampl_offset),
                math.ceil(power_range.stop() + ampl_offset),
                power_range.step()
            )
            self.vprint("Updated power range to {:.2f} ... {:.2f} dBm.".format(
                self.gain_range.start(), self.gain_range.stop()))


def setup_argparser():
    """
    Create argument parser for signal generator.
    """
    parser = UHDApp.setup_argparser(
        description="USRP Signal Generator.",
        tx_or_rx="Tx",
    )
    group = parser.add_argument_group('Siggen Arguments')
    group.add_argument("-m", "--amplitude", type=eng_arg.eng_float, default=0.15,
                       help="Set output amplitude to AMPL (0.0-1.0). Note that "
                            "if --power is given, UHD will attempt to match the "
                            "output power regardless of the amplitude.",
                       metavar="AMPL")
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
        sys.exit(1)
    tb.start()
    input('[UHD-SIGGEN] Press Enter to quit:\n')
    tb.stop()
    tb.wait()


if __name__ == "__main__":
    main()
