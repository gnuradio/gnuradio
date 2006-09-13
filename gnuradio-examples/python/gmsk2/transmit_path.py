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
from pick_bitrate import pick_tx_bitrate

# /////////////////////////////////////////////////////////////////////////////
#                              transmit path
# /////////////////////////////////////////////////////////////////////////////

class transmit_path(gr.hier_block): 
    def __init__(self, fg, mod_class, tx_subdev_spec,
                 bitrate, interp, spb, gain,
                 options, mod_kwargs):

        self.normal_gain = gain

        self.u = usrp.sink_c (fusb_block_size=options.fusb_block_size,
                              fusb_nblocks=options.fusb_nblocks)
        dac_rate = self.u.dac_rate();

        print mod_class
        print mod_class.bits_per_baud()
        (self._bitrate, self._spb, self._interp) = \
            pick_tx_bitrate(bitrate, mod_class.bits_per_baud(), spb, interp, dac_rate)

        self.u.set_interp_rate(self._interp)

        # determine the daughterboard subdevice we're using
        if tx_subdev_spec is None:
            tx_subdev_spec = usrp.pick_tx_subdevice(self.u)
        self.u.set_mux(usrp.determine_tx_mux_value(self.u, tx_subdev_spec))
        self.subdev = usrp.selected_subdev(self.u, tx_subdev_spec)
        print "Using TX d'board %s" % (self.subdev.side_and_name(),)

        # transmitter
        self.packet_transmitter = \
            blks.mod_pkts(fg,
                          mod_class(fg, spb=self._spb, **mod_kwargs),
                          access_code=None,
                          msgq_limit=4,
                          pad_for_usrp=True)

        self.amp = gr.multiply_const_cc (self.normal_gain)

        fg.connect(self.packet_transmitter, self.amp, self.u)
        gr.hier_block.__init__(self, fg, None, None)

        self.set_gain(self.subdev.gain_range()[1])  # set max Tx gain
        self.set_auto_tr(True)                      # enable Auto Transmit/Receive switching

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
        self.gain = gain
        self.subdev.set_gain(gain)

    def set_auto_tr(self, enable):
        return self.subdev.set_auto_tr(enable)
        
    def send_pkt(self, payload='', eof=False):
        return self.packet_transmitter.send_pkt(payload, eof)
        
    def bitrate(self):
        return self._bitrate

    def spb(self):
        return self._spb

    def interp(self):
        return self._interp
