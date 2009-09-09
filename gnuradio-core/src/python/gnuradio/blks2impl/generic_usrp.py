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

USRP1_TYPE = 'usrp1'
USRP2_TYPE = 'usrp2'
DUMMY_TYPE = 'dummy'
#usrp2 rates common for decim and interp
_USRP2_RATES = range(4, 128+1, 1) + range(130, 256+1, 2) + range(260, 512+1, 4)
#dummy common rates
_DUMMY_XRATES = range(4, 512, 2)
_DUMMY_CONVERTER_RATE = 100e6
#dummy freq result
class _dummy_freq_result(object):
    def __init__(self, target_freq):
        self.baseband_freq = target_freq
        self.dxc_freq = 0
        self.residual_freq = 0
from gnuradio import gr, usrp, usrp2

########################################################################
# generic usrp common stuff
########################################################################
class _generic_usrp_base(object):

    def __init__(self, which=0, subdev_spec=None, interface="", mac_addr="",
        fusb_block_size=0, fusb_nblocks=0, usrpx=None, lo_offset=None, gain=None):
        self._lo_offset = lo_offset
        #usrp options
        self._which = which
        self._subdev_spec = subdev_spec
        #usrp2 options
        self._interface = interface
        self._mac_addr = mac_addr
        #fusb options
        self._fusb_block_size = fusb_block_size
        self._fusb_nblocks = fusb_nblocks
        #pick which usrp model
        if usrpx == '0': self._setup_usrpx(DUMMY_TYPE)
        elif usrpx == '1' or self._subdev_spec: self._setup_usrpx(USRP1_TYPE)
        elif usrpx == '2' or self._mac_addr: self._setup_usrpx(USRP2_TYPE)
        else: #automatic
            try: self._setup_usrpx(USRP2_TYPE)
            except:
                try: self._setup_usrpx(USRP1_TYPE)
                except: raise Exception, 'Failed to automatically setup a usrp device.'
        #post usrp setup
        if self._lo_offset is not None:
            self.set_lo_offset(self._lo_offset)
        self.set_gain(gain)
        self.set_auto_tr(True)

    def _setup_usrpx(self, type):
        """
        Call the appropriate setup method.
        @param type the usrp type constant
        """
        self._type = type
        if self._type == USRP1_TYPE: self._setup_usrp1()
        elif self._type == USRP2_TYPE: self._setup_usrp2()
        elif self._type == DUMMY_TYPE: self._setup_dummy()

    def __str__(self):
        if self._type == USRP1_TYPE: return self._subdev.side_and_name()
        elif self._type == USRP2_TYPE:
            return 'Interface: %s    MAC Address: %s    D-Board ID: 0x%.2x'%(
                self._u.interface_name(), self._u.mac_addr(), self._u.daughterboard_id())
        elif self._type == DUMMY_TYPE: return 'Dummy USRP Device'

    def gain(self): return self._gain

    def set_gain(self, gain=None):
        #automatic gain calculation
        r = self.gain_range()
        if gain is None: gain = (r[0] + r[1])/2 # set gain to midpoint
        #set gain for usrp
        self._gain = gain
        if self._type == USRP1_TYPE: return self._subdev.set_gain(gain)
        elif self._type == USRP2_TYPE: return self._u.set_gain(gain)
        elif self._type == DUMMY_TYPE: return True

    def gain_range(self):
        if self._type == USRP1_TYPE: return self._subdev.gain_range()
        elif self._type == USRP2_TYPE: return self._u.gain_range()
        elif self._type == DUMMY_TYPE: return (0, 0, 0)

    def set_center_freq(self, target_freq):
        if self._type == USRP1_TYPE:
            return self._u.tune(self._dxc, self._subdev, target_freq)
        elif self._type == USRP2_TYPE:
            return self._u.set_center_freq(target_freq)
        elif self._type == DUMMY_TYPE: return _dummy_freq_result(target_freq)

    def freq_range(self):
        if self._type == USRP1_TYPE: return self._subdev.freq_range()
        elif self._type == USRP2_TYPE: return self._u.freq_range()
        elif self._type == DUMMY_TYPE: return (-10e9, 10e9, 100e3)

    def set_lo_offset(self, lo_offset):
        if self._type == USRP1_TYPE: return self._subdev.set_lo_offset(lo_offset)
        elif self._type == USRP2_TYPE: return self._u.set_lo_offset(lo_offset)
        elif self._type == DUMMY_TYPE: return True

    def set_auto_tr(self, enable):
        if self._type == USRP1_TYPE: return self._subdev.set_auto_tr(enable)

    def __del__(self):
        try: # Avoid weak reference error
            del self._u
            del self._subdev
        except: pass

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
        gr.hier_block2.__init__(self, "generic_usrp_source",
            gr.io_signature(0, 0, 0), # Input signature
            gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature
        _generic_usrp_base.__init__(self, **kwargs)
        self.connect(self._u, self)

    ####################################################################
    # generic access methods
    ####################################################################
    def set_decim(self, decim):
        if decim not in self.get_decim_rates(): return False
        if self._type == USRP1_TYPE: return self._u.set_decim_rate(decim)
        elif self._type == USRP2_TYPE: return self._u.set_decim(decim)
        elif self._type == DUMMY_TYPE: return True

    def get_decim_rates(self):
        if self._type == USRP1_TYPE: return range(8, 256+1, 2) #default firmware w/ hb filters
        if self._type == USRP2_TYPE: return _USRP2_RATES
        elif self._type == DUMMY_TYPE: return _DUMMY_XRATES

    def adc_rate(self):
        if self._type == USRP1_TYPE: return self._u.adc_rate()
        if self._type == USRP2_TYPE: return self._u.adc_rate()
        elif self._type == DUMMY_TYPE: return _DUMMY_CONVERTER_RATE

    ####################################################################
    # setup usrp methods
    ####################################################################
    def _setup_usrp1(self):
        self._u = usrp.source_c (self._which,
                                fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        # determine the daughterboard subdevice we're using
        if self._subdev_spec is None:
            self._subdev_spec = usrp.pick_rx_subdevice(self._u)
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
        self._u.set_mux(usrp.determine_rx_mux_value(self._u, self._subdev_spec))
        self._dxc = 0

    def _setup_usrp2(self):
        self._u = usrp2.source_32fc(self._interface, self._mac_addr)

    def _setup_dummy(self): self._u = gr.null_source(gr.sizeof_gr_complex)

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
        gr.hier_block2.__init__(self, "generic_usrp_sink",
            gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
            gr.io_signature(0, 0, 0)) # Output signature
        _generic_usrp_base.__init__(self, **kwargs)
        if self._type == USRP1_TYPE: #scale 0.0 to 1.0 input for usrp1
            self.connect(self, gr.multiply_const_cc((2**15)-1), self._u)
        else: self.connect(self, self._u)

    ####################################################################
    # generic access methods
    ####################################################################
    def set_interp(self, interp):
        if interp not in self.get_interp_rates(): return False
        if self._type == USRP1_TYPE: return self._u.set_interp_rate(interp)
        elif self._type == USRP2_TYPE: return self._u.set_interp(interp)
        elif self._type == DUMMY_TYPE: return True

    def get_interp_rates(self):
        if self._type == USRP1_TYPE: return range(16, 512+1, 4)
        if self._type == USRP2_TYPE: return _USRP2_RATES
        elif self._type == DUMMY_TYPE: return _DUMMY_XRATES

    def dac_rate(self):
        if self._type == USRP1_TYPE: return self._u.dac_rate()
        if self._type == USRP2_TYPE: return self._u.dac_rate()
        elif self._type == DUMMY_TYPE: return _DUMMY_CONVERTER_RATE

    ####################################################################
    # setup usrp methods
    ####################################################################
    def _setup_usrp1(self):
        self._u = usrp.sink_c (self._which,
                                fusb_block_size=self._fusb_block_size,
                                fusb_nblocks=self._fusb_nblocks)
        # determine the daughterboard subdevice we're using
        if self._subdev_spec is None:
            self._subdev_spec = usrp.pick_tx_subdevice(self._u)
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
        self._u.set_mux(usrp.determine_tx_mux_value(self._u, self._subdev_spec))
        self._dxc = self._subdev.which()

    def _setup_usrp2(self): self._u = usrp2.sink_32fc(self._interface, self._mac_addr)

    def _setup_dummy(self): self._u = gr.null_sink(gr.sizeof_gr_complex)
