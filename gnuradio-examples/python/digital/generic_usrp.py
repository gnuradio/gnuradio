#
# Copyright 2009 Free Software Foundation, Inc.
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

USRP_TYPE = 'usrp'
USRP2_TYPE = 'usrp2'

from gnuradio import gr, usrp, usrp2

########################################################################
# generic usrp common stuff
########################################################################
class _generic_usrp_base(object):

    def __init__(self, which=0, subdev_spec=None, interface="", mac_addr="", fusb_block_size=0, fusb_nblocks=0, usrpx=None):
        self._gain = 0
        self._usrpx = usrpx
        #usrp options
        self._which = which
        self._subdev_spec = subdev_spec
        #usrp2 options
        self._interface = interface
        self._mac_addr = mac_addr
        #fusb options
        self._fusb_block_size = fusb_block_size
        self._fusb_nblocks = fusb_nblocks

    def __str__(self):
        if self._type == USRP_TYPE: return self._subdev.side_and_name()
        elif self._type == USRP2_TYPE: return "D-Board ID 0x%x\n"%self._u.daughterboard_id()

    def gain(self): return self._gain

    def set_gain(self, gain=None):
        if gain is None:
            r = self.gain_range()
            gain = (r[0] + r[1])/2               # set gain to midpoint
        self._gain = gain
        if self._type == USRP_TYPE: return self._subdev.set_gain(gain)
        elif self._type == USRP2_TYPE: return self._u.set_gain(gain)

    def gain_range(self):
        if self._type == USRP_TYPE: return self._subdev.gain_range()
        elif self._type == USRP2_TYPE: return self._u.gain_range()

    def set_center_freq(self, target_freq):
        if self._type == USRP_TYPE:
            return bool(self._u.tune(self._dxc, self._subdev, target_freq))
        elif self._type == USRP2_TYPE:
            return self._u.set_center_freq(target_freq)

    def set_auto_tr(self, enable):
        if self._type == USRP_TYPE: return self._subdev.set_auto_tr(enable)

    def __del__(self):
        # Avoid weak reference error
        if self._type == USRP_TYPE: del self._subdev

########################################################################
# generic usrp source
########################################################################
class generic_usrp_source_c(_generic_usrp_base, gr.hier_block2):
    """
    Create a generic usrp source that represents usrp and usrp2.
    Take usrp and usrp2 constructor arguments and try to figure out usrp or usrp2.
    Provide generic access methods so the API looks the same for both.
    """

    def __init__(self, **kwargs):
        _generic_usrp_base.__init__(self, **kwargs)
        gr.hier_block2.__init__(self, "generic_usrp_source",
            gr.io_signature(0, 0, 0), # Input signature
            gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature
        #pick usrp or usrp2
        if self._usrpx == '1' or self._subdev_spec:
            self._setup_usrp_source()
        elif self._usrpx == '2' or self._mac_addr:
            self._setup_usrp2_source()
        else: #automatic
            try: self._setup_usrp2_source()
            except: self._setup_usrp_source()
        self.connect(self._u, self)
        self.set_auto_tr(True)

    ####################################################################
    # generic access methods
    ####################################################################
    def set_decim(self, decim):
        if self._type == USRP_TYPE: return self._u.set_decim_rate(decim)
        elif self._type == USRP2_TYPE: return self._u.set_decim(decim)

    def adc_rate(self): return self._u.adc_rate()

    ####################################################################
    # setup usrp methods
    ####################################################################
    def _setup_usrp_source(self):
        self._u = usrp.source_c (self._which,
                                fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        # determine the daughterboard subdevice we're using
        if self._subdev_spec is None:
            self._subdev_spec = usrp.pick_rx_subdevice(self._u)
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
        self._u.set_mux(usrp.determine_rx_mux_value(self._u, self._subdev_spec))
        self._type = USRP_TYPE
        self._dxc = 0

    def _setup_usrp2_source(self):
        self._u = usrp2.source_32fc(self._interface, self._mac_addr)
        self._type = USRP2_TYPE

########################################################################
# generic usrp sink
########################################################################
class generic_usrp_sink_c(_generic_usrp_base, gr.hier_block2):
    """
    Create a generic usrp sink that represents usrp and usrp2.
    Take usrp and usrp2 constructor arguments and try to figure out usrp or usrp2.
    Provide generic access methods so the API looks the same for both.
    """

    def __init__(self, **kwargs):
        _generic_usrp_base.__init__(self, **kwargs)
        gr.hier_block2.__init__(self, "generic_usrp_sink",
            gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
            gr.io_signature(0, 0, 0)) # Output signature

        #pick usrp or usrp2
        if self._usrpx == '1' or self._subdev_spec:
            self._setup_usrp_source()
        elif self._usrpx == '2' or self._mac_addr:
            self._setup_usrp2_source()
        else: #automatic
            try: self._setup_usrp2_source()
            except: self._setup_usrp_source()
        self.connect(self, self._u)
        self.set_auto_tr(True)

    ####################################################################
    # generic access methods
    ####################################################################
    def set_interp(self, interp):
        if self._type == USRP_TYPE: return self._u.set_interp_rate(interp)
        elif self._type == USRP2_TYPE: return self._u.set_interp(interp)

    def dac_rate(self): return self._u.dac_rate()

    def ampl_range(self):
        if self._type == USRP_TYPE: return (0.0, 2.**15-1)
        elif self._type == USRP2_TYPE: return (0.0, 1.0)

    ####################################################################
    # setup usrp methods
    ####################################################################
    def _setup_usrp_source(self):
        self._u = usrp.sink_c (self._which,
                                fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        # determine the daughterboard subdevice we're using
        if self._subdev_spec is None:
            self._subdev_spec = usrp.pick_tx_subdevice(self._u)
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
        self._u.set_mux(usrp.determine_tx_mux_value(self._u, self._subdev_spec))
        self._type = USRP_TYPE
        self._dxc = self._subdev.which()

    def _setup_usrp2_source(self):
        self._u = usrp2.sink_32fc(self._interface, self._mac_addr)
        self._type = USRP2_TYPE
