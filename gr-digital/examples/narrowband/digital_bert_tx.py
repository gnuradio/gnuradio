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
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

from gnuradio import digital

# from current dir
from uhd_interface import uhd_transmitter

n2s = eng_notation.num_to_str

class bert_transmit(gr.hier_block2):
    def __init__(self, constellation, samples_per_symbol,
                 differential, excess_bw, gray_coded,
                 verbose, log):

        gr.hier_block2.__init__(self, "bert_transmit",
                                gr.io_signature(0, 0, 0),                    # Output signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Input signature
        
        # Create BERT data bit stream	
	self._bits = gr.vector_source_b([1,], True)      # Infinite stream of ones
        self._scrambler = gr.scrambler_bb(0x8A, 0x7F, 7) # CCSDS 7-bit scrambler

        self._mod = digital.generic_mod(constellation, samples_per_symbol,
                                        differential, excess_bw, gray_coded,
                                        verbose, log)

        self._pack = gr.unpacked_to_packed_bb(self._mod.bits_per_symbol(), gr.GR_MSB_FIRST)

        self.connect(self._bits, self._scrambler, self._pack, self._mod, self)


class tx_psk_block(gr.top_block):
    def __init__(self, mod, options):
	gr.top_block.__init__(self, "tx_mpsk")

        self._modulator_class = mod

        # Get mod_kwargs
        mod_kwargs = self._modulator_class.extract_kwargs_from_options(options)
        
        # transmitter
	self._modulator = self._modulator_class(**mod_kwargs)

        if(options.tx_freq is not None):
            symbol_rate = options.bitrate / self._modulator.bits_per_symbol()
            self._sink = uhd_transmitter(options.args, symbol_rate,
                                         options.samples_per_symbol,
                                         options.tx_freq, options.tx_gain,
                                         options.spec,
                                         options.antenna, options.verbose)
            options.samples_per_symbol = self._sink._sps
            
        elif(options.to_file is not None):
            self._sink = gr.file_sink(gr.sizeof_gr_complex, options.to_file)
        else:
            self._sink = gr.null_sink(gr.sizeof_gr_complex)
            
            
        self._transmitter = bert_transmit(self._modulator._constellation,
                                          options.samples_per_symbol,
                                          options.differential,
                                          options.excess_bw,
                                          gray_coded=True,
                                          verbose=options.verbose,
                                          log=options.log)

        self.amp = gr.multiply_const_cc(options.amplitude)
	self.connect(self._transmitter, self.amp, self._sink)


def get_options(mods):
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='psk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))
    parser.add_option("", "--amplitude", type="eng_float", default=0.2,
                      help="set Tx amplitude (0-1) (default=%default)")
    parser.add_option("-r", "--bitrate", type="eng_float", default=250e3,
                      help="Select modulation bit rate (default=%default)")
    parser.add_option("-S", "--samples-per-symbol", type="float", default=2,
                      help="set samples/symbol [default=%default]")
    parser.add_option("","--to-file", default=None,
                      help="Output file for modulated samples")
    if not parser.has_option("--verbose"):
        parser.add_option("-v", "--verbose", action="store_true", default=False)
    if not parser.has_option("--log"):
        parser.add_option("", "--log", action="store_true", default=False)

    uhd_transmitter.add_options(parser)

    for mod in mods.values():
        mod.add_options(parser)
		      
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
	
    return (options, args)

if __name__ == "__main__":
    mods = digital.modulation_utils.type_1_mods()

    (options, args) = get_options(mods)
    
    mod = mods[options.modulation]
    tb = tx_psk_block(mod, options)

    try:
        tb.run()
    except KeyboardInterrupt:
        pass
