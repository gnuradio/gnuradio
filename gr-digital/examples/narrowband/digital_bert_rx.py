#!/usr/bin/env python
#
# Copyright 2008,2011 Free Software Foundation, Inc.
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
from optparse import OptionParser
from gnuradio.eng_option import eng_option
import gnuradio.gr.gr_threading as _threading
import sys, time, math

from gnuradio import digital

# from current dir
from uhd_interface import uhd_receiver

n2s = eng_notation.num_to_str

class status_thread(_threading.Thread):
    def __init__(self, tb):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.tb = tb
        self.done = False
        self.start()

    def run(self):
        while not self.done:
            print "Freq. Offset: {0:5.0f} Hz  Timing Offset: {1:10.1f} ppm  Estimated SNR: {2:4.1f} dB  BER: {3:g}".format(
                tb.frequency_offset(), tb.timing_offset()*1e6, tb.snr(), tb.ber())
            try:
                time.sleep(1.0)
            except KeyboardInterrupt:
                self.done = True



class bert_receiver(gr.hier_block2):
    def __init__(self, bitrate,
                 constellation, samples_per_symbol,
                 differential, excess_bw, gray_coded,
                 freq_bw, timing_bw, phase_bw,
                 verbose, log):

        gr.hier_block2.__init__(self, "bert_receive",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(0, 0, 0))                    # Output signature
        
        self._bitrate = bitrate

        self._demod = digital.generic_demod(constellation, samples_per_symbol,
                                            differential, excess_bw, gray_coded,
                                            freq_bw, timing_bw, phase_bw,
                                            verbose, log)

        self._symbol_rate = self._bitrate / self._demod.bits_per_symbol()
        self._sample_rate = self._symbol_rate * samples_per_symbol

        # Add an SNR probe on the demodulated constellation
        self._snr_probe = digital.probe_mpsk_snr_est_c(digital.SNR_EST_M2M4, alpha=10.0/self._symbol_rate)
        self.connect(self._demod.time_recov, self._snr_probe)
        
        # Descramble BERT sequence.  A channel error will create 3 incorrect bits
        self._descrambler = gr.descrambler_bb(0x8A, 0x7F, 7) # CCSDS 7-bit descrambler

        # Measure BER by the density of 0s in the stream
        self._ber = gr.probe_density_b(1.0/self._symbol_rate)
        
        self.connect(self, self._demod, self._descrambler, self._ber)

    def frequency_offset(self):
        return self._demod.freq_recov.get_frequency()*self._sample_rate/(2*math.pi)

    def timing_offset(self):
        return self._demod.time_recov.get_clock_rate()

    def snr(self):
        return self._snr_probe.snr()

    def ber(self):
        return (1.0-self._ber.density())/3.0



class rx_psk_block(gr.top_block):
    def __init__(self, demod, options):

	gr.top_block.__init__(self, "rx_mpsk")

        self._demodulator_class = demod

        # Get demod_kwargs
        demod_kwargs = self._demodulator_class.extract_kwargs_from_options(options)
        
        # demodulator
	self._demodulator = self._demodulator_class(**demod_kwargs)

        if(options.rx_freq is not None):
            symbol_rate = options.bitrate / self._demodulator.bits_per_symbol()
            self._source = uhd_receiver(options.args, options.bitrate,
                                        options.samples_per_symbol,
                                        options.rx_freq, options.rx_gain,
                                        options.spec,
                                        options.antenna, options.verbose)
            options.samples_per_symbol = self._source._sps

        elif(options.from_file is not None):
            self._source = gr.file_source(gr.sizeof_gr_complex, options.from_file)
        else:
            self._source = gr.null_source(gr.sizeof_gr_complex)

        # Create the BERT receiver
        self._receiver = bert_receiver(options.bitrate,
                                       self._demodulator._constellation, 
                                       options.samples_per_symbol,
                                       options.differential, 
                                       options.excess_bw, 
                                       gray_coded=True,
                                       freq_bw=options.freq_bw,
                                       timing_bw=options.timing_bw,
                                       phase_bw=options.phase_bw,
                                       verbose=options.verbose,
                                       log=options.log)
        
        self.connect(self._source, self._receiver)

    def snr(self):
        return self._receiver.snr()

    def mag(self):
        return self._receiver.signal_mean()
        
    def var(self):
        return self._receiver.noise_variance()

    def ber(self):
        return self._receiver.ber()

    def frequency_offset(self):
        return self._receiver.frequency_offset()
        
    def timing_offset(self):
        return self._receiver.timing_offset()
            

def get_options(demods):
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("","--from-file", default=None,
                      help="input file of samples to demod")
    parser.add_option("-m", "--modulation", type="choice", choices=demods.keys(), 
                      default='psk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(demods.keys()),))
    parser.add_option("-r", "--bitrate", type="eng_float", default=250e3,
                      help="Select modulation bit rate (default=%default)")
    parser.add_option("-S", "--samples-per-symbol", type="float", default=2,
                      help="set samples/symbol [default=%default]")
    if not parser.has_option("--verbose"):
        parser.add_option("-v", "--verbose", action="store_true", default=False)
    if not parser.has_option("--log"):
        parser.add_option("", "--log", action="store_true", default=False,
                      help="Log all parts of flow graph to files (CAUTION: lots of data)")

    uhd_receiver.add_options(parser)

    demods = digital.modulation_utils.type_1_demods()
    for mod in demods.values():
        mod.add_options(parser)
		      
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
	
    return (options, args)


if __name__ == "__main__":
    demods = digital.modulation_utils.type_1_demods()

    (options, args) = get_options(demods)

    demod = demods[options.modulation]
    tb = rx_psk_block(demod, options)

    print "\n*** SNR estimator is inaccurate below about 7dB"
    print "*** BER estimator is inaccurate above about 10%\n"
    updater = status_thread(tb)

    try:
        tb.run()
    except KeyboardInterrupt:
        updater.done = True
        updater = None
