#!/usr/bin/env python
#
# Copyright 2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, gru, blks
from gnuradio import usrp

# from current dir
from pick_bitrate import pick_rx_bitrate

# /////////////////////////////////////////////////////////////////////////////
#                              receive path
# /////////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block):
    def __init__(self, fg, demod_class, rx_subdev_spec,
                 bitrate, decim, spb,
                 rx_callback, options, demod_kwargs):

        self.u = usrp.source_c (fusb_block_size=options.fusb_block_size,
                                fusb_nblocks=options.fusb_nblocks)
        adc_rate = self.u.adc_rate()

        (self._bitrate, self._spb, self._decim) = \
            pick_rx_bitrate(bitrate, demod_class.bits_per_baud(), spb, decim, adc_rate)

        self.u.set_decim_rate(self._decim)
        sw_decim = 1

        if rx_subdev_spec is None:
            rx_subdev_spec = usrp.pick_rx_subdevice(self.u)
        self.subdev = usrp.selected_subdev(self.u, rx_subdev_spec)
        print "Using RX d'board %s" % (self.subdev.side_and_name(),)

        self.u.set_mux(usrp.determine_rx_mux_value(self.u, rx_subdev_spec))

        # Create filter to get actual channel we want
        chan_coeffs = gr.firdes.low_pass (1.0,                  # gain
                                          sw_decim * self._spb, # sampling rate
                                          1.0,                  # midpoint of trans. band
                                          0.1,                  # width of trans. band
                                          gr.firdes.WIN_HANN)   # filter type 

        print "len(rx_chan_coeffs) =", len(chan_coeffs)

        # Decimating Channel filter
        # complex in and out, float taps
        self.chan_filt = gr.fft_filter_ccc(sw_decim, chan_coeffs)
        #self.chan_filt = gr.fir_filter_ccf(sw_decim, chan_coeffs)

        # receiver
        self.packet_receiver = \
            blks.demod_pkts(fg,
                            demod_class(fg, spb=self._spb, **demod_kwargs),
                            access_code=None,
                            callback=rx_callback,
                            threshold=-1)

        fg.connect(self.u, self.chan_filt, self.packet_receiver)
        gr.hier_block.__init__(self, fg, None, None)

        g = self.subdev.gain_range()
        #self.set_gain((g[0] + g[1])/2)        # set gain to midpoint
        self.set_gain(g[1])                    # set gain to max
        self.set_auto_tr(True)                 # enable Auto Transmit/Receive switching

        # Carrier Sensing Blocks
        alpha = 0.001
        thresh = 30   # in dB, will have to adjust
        self.probe = gr.probe_avg_mag_sqrd_c(thresh,alpha)
        fg.connect(self.chan_filt, self.probe)

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
        if gain is None:
            r = self.subdev.gain_range()
            gain = (r[0] + r[1])/2               # set gain to midpoint
        self.gain = gain
        return self.subdev.set_gain(gain)

    def set_auto_tr(self, enable):
        return self.subdev.set_auto_tr(enable)
        
    def bitrate(self):
        return self._bitrate

    def spb(self):
        return self._spb

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
    
