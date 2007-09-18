#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, blks2
from gnuradio import usrp
from gnuradio import eng_notation

import copy
import sys

# from current dir
from pick_bitrate import pick_tx_bitrate

# /////////////////////////////////////////////////////////////////////////////
#                              transmit path
# /////////////////////////////////////////////////////////////////////////////

class transmit_path(gr.hier_block2): 
    def __init__(self, modulator_class, options):
        '''
        See below for what options should hold
        '''
	gr.hier_block2.__init__(self, "transmit_path",
                                gr.io_signature(0, 0, 0), # Input signature
                                gr.io_signature(0, 0, 0)) # Output signature

        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose            = options.verbose
        self._tx_freq            = options.tx_freq         # tranmitter's center frequency
        self._tx_amplitude       = options.tx_amplitude    # digital amplitude sent to USRP
        self._tx_subdev_spec     = options.tx_subdev_spec  # daughterboard to use
        self._bitrate            = options.bitrate         # desired bit rate
        self._interp             = options.interp          # interpolating rate for the USRP (prelim)
        self._samples_per_symbol = options.samples_per_symbol  # desired samples/baud
        self._fusb_block_size    = options.fusb_block_size # usb info for USRP
        self._fusb_nblocks       = options.fusb_nblocks    # usb info for USRP
        self._use_whitener_offset = options.use_whitener_offset # increment start of whitener XOR data
        
        self._modulator_class = modulator_class         # the modulator_class we are using
    
        if self._tx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --tx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP sink; also adjusts interp, samples_per_symbol, and bitrate
        self._setup_usrp_sink()

        # copy the final answers back into options for use by modulator
        options.samples_per_symbol = self._samples_per_symbol
        options.bitrate = self._bitrate
        options.interp = self._interp

        # Get mod_kwargs
        mod_kwargs = self._modulator_class.extract_kwargs_from_options(options)

        # Set center frequency of USRP
        ok = self.set_freq(self._tx_freq)
        if not ok:
            print "Failed to set Tx frequency to %s" % (eng_notation.num_to_str(self._tx_freq),)
            raise ValueError
    
        # transmitter
        self.packet_transmitter = \
            blks2.mod_pkts(self._modulator_class(**mod_kwargs),
                           access_code=None,
                           msgq_limit=4,
                           pad_for_usrp=True,
                           use_whitener_offset=options.use_whitener_offset)


        # Set the USRP for maximum transmit gain
        # (Note that on the RFX cards this is a nop.)
        self.set_gain(self.subdev.gain_range()[1])

        self.amp = gr.multiply_const_cc(1)
        self.set_tx_amplitude(self._tx_amplitude)

        # enable Auto Transmit/Receive switching
        self.set_auto_tr(True)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

        # Create and setup transmit path flow graph
        self.connect(self.packet_transmitter, self.amp, self.u)

    def _setup_usrp_sink(self):
        """
        Creates a USRP sink, determines the settings for best bitrate,
        and attaches to the transmitter's subdevice.
        """
        self.u = usrp.sink_c(fusb_block_size=self._fusb_block_size,
                             fusb_nblocks=self._fusb_nblocks)
        dac_rate = self.u.dac_rate();

        # derive values of bitrate, samples_per_symbol, and interp from desired info
        (self._bitrate, self._samples_per_symbol, self._interp) = \
            pick_tx_bitrate(self._bitrate, self._modulator_class.bits_per_symbol(),
                            self._samples_per_symbol, self._interp, dac_rate)
        
        self.u.set_interp_rate(self._interp)

        # determine the daughterboard subdevice we're using
        if self._tx_subdev_spec is None:
            self._tx_subdev_spec = usrp.pick_tx_subdevice(self.u)
        self.u.set_mux(usrp.determine_tx_mux_value(self.u, self._tx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, self._tx_subdev_spec)


    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """
        r = self.u.tune(self.subdev._which, self.subdev, target_freq)
        if r:
            return True

        return False
        
    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        self.gain = gain
        self.subdev.set_gain(gain)

    def set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP
        @param: ampl 0 <= ampl < 32768.  Try 8000
        """
        self._tx_amplitude = max(0.0, min(ampl, 32767.0))
        self.amp.set_k(self._tx_amplitude)
        
    def set_auto_tr(self, enable):
        """
        Turns on auto transmit/receive of USRP daughterboard (if exits; else ignored)
        """
        return self.subdev.set_auto_tr(enable)
        
    def send_pkt(self, payload='', eof=False):
        """
        Calls the transmitter method to send a packet
        """
        return self.packet_transmitter.send_pkt(payload, eof)
        
    def bitrate(self):
        return self._bitrate

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def interp(self):
        return self._interp

    def add_options(normal, expert):
        """
        Adds transmitter-specific options to the Options Parser
        """
        add_freq_option(normal)
        if not normal.has_option('--bitrate'):
            normal.add_option("-r", "--bitrate", type="eng_float", default=None,
                              help="specify bitrate.  samples-per-symbol and interp/decim will be derived.")
        normal.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Tx side A or B")
        normal.add_option("", "--tx-amplitude", type="eng_float", default=12000, metavar="AMPL",
                          help="set transmitter digital amplitude: 0 <= AMPL < 32768 [default=%default]")
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("-S", "--samples-per-symbol", type="int", default=None,
                          help="set samples/symbol [default=%default]")
        expert.add_option("", "--tx-freq", type="eng_float", default=None,
                          help="set transmit frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-i", "--interp", type="intx", default=None,
                          help="set fpga interpolation rate to INTERP [default=%default]")
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to file (CAUTION: lots of data)")
        expert.add_option("","--use-whitener-offset", action="store_true", default=False,
                          help="make sequential packets use different whitening")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

    def _print_verbage(self):
        """
        Prints information about the transmit path
        """
        print "Using TX d'board %s"    % (self.subdev.side_and_name(),)
        print "Tx amplitude     %s"    % (self._tx_amplitude)
        print "modulation:      %s"    % (self._modulator_class.__name__)
        print "bitrate:         %sb/s" % (eng_notation.num_to_str(self._bitrate))
        print "samples/symbol:  %3d"   % (self._samples_per_symbol)
        print "interp:          %3d"   % (self._interp)
        print "Tx Frequency:    %s"    % (eng_notation.num_to_str(self._tx_freq))
        

def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")
