#!/usr/bin/env python
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
from pick_bitrate import pick_rx_bitrate

# /////////////////////////////////////////////////////////////////////////////
#                              receive path
# /////////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block2):
    def __init__(self, demod_class, rx_callback, options):

	gr.hier_block2.__init__(self, "receive_path",
                                gr.io_signature(0, 0, 0), # Input signature
                                gr.io_signature(0, 0, 0)) # Output signature

        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose            = options.verbose
        self._rx_freq            = options.rx_freq         # receiver's center frequency
        self._rx_gain            = options.rx_gain         # receiver's gain
        self._rx_subdev_spec     = options.rx_subdev_spec  # daughterboard to use
        self._bitrate            = options.bitrate         # desired bit rate
        self._decim              = options.decim           # Decimating rate for the USRP (prelim)
        self._samples_per_symbol = options.samples_per_symbol  # desired samples/symbol
        self._fusb_block_size    = options.fusb_block_size # usb info for USRP
        self._fusb_nblocks       = options.fusb_nblocks    # usb info for USRP

        self._rx_callback   = rx_callback      # this callback is fired when there's a packet available
        self._demod_class   = demod_class      # the demodulator_class we're using

        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP source; also adjusts decim, samples_per_symbol, and bitrate
        self._setup_usrp_source()

        g = self.subdev.gain_range()
        if options.show_rx_gain_range:
            print "Rx Gain Range: minimum = %g, maximum = %g, step size = %g" \
                  % (g[0], g[1], g[2])

        self.set_gain(options.rx_gain)

        self.set_auto_tr(True)                 # enable Auto Transmit/Receive switching

        # Set RF frequency
        ok = self.set_freq(self._rx_freq)
        if not ok:
            print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(self._rx_freq))
            raise ValueError, eng_notation.num_to_str(self._rx_freq)

        # copy the final answers back into options for use by demodulator
        options.samples_per_symbol = self._samples_per_symbol
        options.bitrate = self._bitrate
        options.decim = self._decim

        # Get demod_kwargs
        demod_kwargs = self._demod_class.extract_kwargs_from_options(options)

        # Design filter to get actual channel we want
        sw_decim = 1
        chan_coeffs = gr.firdes.low_pass (1.0,                  # gain
                                          sw_decim * self._samples_per_symbol, # sampling rate
                                          1.0,                  # midpoint of trans. band
                                          0.5,                  # width of trans. band
                                          gr.firdes.WIN_HANN)   # filter type 

        # Decimating channel filter
        # complex in and out, float taps
        self.chan_filt = gr.fft_filter_ccc(sw_decim, chan_coeffs)
        #self.chan_filt = gr.fir_filter_ccf(sw_decim, chan_coeffs)

        # receiver
        self.packet_receiver = \
            blks2.demod_pkts(self._demod_class(**demod_kwargs),
                             access_code=None,
                             callback=self._rx_callback,
                             threshold=-1)
    
        # Carrier Sensing Blocks
        alpha = 0.001
        thresh = 30   # in dB, will have to adjust

        if options.log_rx_power == True:
            self.probe = gr.probe_avg_mag_sqrd_cf(thresh,alpha)
            self.power_sink = gr.file_sink(gr.sizeof_float, "rxpower.dat")
            self.connect(self.chan_filt, self.probe, self.power_sink)
        else:
            self.probe = gr.probe_avg_mag_sqrd_c(thresh,alpha)
            self.connect(self.chan_filt, self.probe)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()
            
        self.connect(self.u, self.chan_filt, self.packet_receiver)

    def _setup_usrp_source(self):
        self.u = usrp.source_c (fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        adc_rate = self.u.adc_rate()

        # derive values of bitrate, samples_per_symbol, and decim from desired info
        (self._bitrate, self._samples_per_symbol, self._decim) = \
            pick_rx_bitrate(self._bitrate, self._demod_class.bits_per_symbol(), \
                            self._samples_per_symbol, self._decim, adc_rate)

        self.u.set_decim_rate(self._decim)

        # determine the daughterboard subdevice we're using
        if self._rx_subdev_spec is None:
            self._rx_subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.subdev = usrp.selected_subdev(self.u, self._rx_subdev_spec)

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, self._rx_subdev_spec))

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
        r = self.u.tune(0, self.subdev, target_freq)
        if r:
            return True

        return False

    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        if gain is None:
            r = self.subdev.gain_range()
            gain = (r[0] + r[1])/2               # set gain to midpoint
        self.gain = gain
        return self.subdev.set_gain(gain)

    def set_auto_tr(self, enable):
        return self.subdev.set_auto_tr(enable)
        
    def bitrate(self):
        return self._bitrate

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def decim(self):
        return self._decim

    def carrier_sensed(self):
        """
        Return True if we think carrier is present.
        """
        #return self.probe.level() > X
        return self.probe.unmuted()

    def carrier_threshold(self):
        """
        Return current setting in dB.
        """
        return self.probe.threshold()

    def set_carrier_threshold(self, threshold_in_db):
        """
        Set carrier threshold.

        @param threshold_in_db: set detection threshold
        @type threshold_in_db:  float (dB)
        """
        self.probe.set_threshold(threshold_in_db)
    
        
    def add_options(normal, expert):
        """
        Adds receiver-specific options to the Options Parser
        """
        add_freq_option(normal)
        if not normal.has_option("--bitrate"):
            normal.add_option("-r", "--bitrate", type="eng_float", default=None,
                              help="specify bitrate.  samples-per-symbol and interp/decim will be derived.")
        normal.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B")
        normal.add_option("", "--rx-gain", type="eng_float", default=None, metavar="GAIN",
                          help="set receiver gain in dB [default=midpoint].  See also --show-rx-gain-range")
        normal.add_option("", "--show-rx-gain-range", action="store_true", default=False, 
                          help="print min and max Rx gain available on selected daughterboard")
        normal.add_option("-v", "--verbose", action="store_true", default=False)
        expert.add_option("-S", "--samples-per-symbol", type="int", default=None,
                          help="set samples/symbol [default=%default]")
        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
        expert.add_option("-d", "--decim", type="intx", default=None,
                          help="set fpga decimation rate to DECIM [default=%default]")
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to files (CAUTION: lots of data)")
        expert.add_option("", "--log-rx-power", action="store_true", default=False,
                          help="Log receive signal power to file (CAUTION: lots of data)")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)


    def _print_verbage(self):
        """
        Prints information about the receive path
        """
        print "\nReceive Path:"
        print "Using RX d'board %s"    % (self.subdev.side_and_name(),)
        print "Rx gain:         %g"    % (self.gain,)
        print "modulation:      %s"    % (self._demod_class.__name__)
        print "bitrate:         %sb/s" % (eng_notation.num_to_str(self._bitrate))
        print "samples/symbol:  %3d"   % (self._samples_per_symbol)
        print "decim:           %3d"   % (self._decim)
        print "Rx Frequency:    %s"    % (eng_notation.num_to_str(self._rx_freq))
        # print "Rx Frequency:    %f"    % (self._rx_freq)

    def __del__(self):
        # Avoid weak reference error
        del self.subdev
            
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
