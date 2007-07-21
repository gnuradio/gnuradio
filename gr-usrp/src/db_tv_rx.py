#
# Copyright 2005 Free Software Foundation, Inc.
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

__all__ = ['tv_rx']

import math
from usrpm import usrp_dbid
import db_base
import db_instantiator

def int_seq_to_str(seq):
    """convert a sequence of integers into a string"""
    return ''.join (map (chr, seq))

def str_to_int_seq(str):
    """convert a string to a list of integers"""
    return map (ord, str)

def control_byte_1(fast_tuning_p, reference_divisor):
    c = 0x88
    if fast_tuning_p:
        c |= 0x40
    if reference_divisor == 512:
        c |= 0x3 << 1
    elif reference_divisor == 640:
        c |= 0x0 << 1
    elif reference_divisor == 1024:
        c |= 0x1 << 1
    else:
        assert 0
    return c

def control_byte_2(target_freq, shutdown_tx_PGA):
    if target_freq < 158e6:        # VHF low
        c = 0xa0
    elif target_freq < 464e6:      # VHF high
        c = 0x90
    else:                          # UHF
        c = 0x30
    if shutdown_tx_PGA:
        c |= 0x08
    return c

class db_tv_rx(db_base.db_base):
    def __init__(self, usrp, which, first_IF, second_IF):
        """
        Control Microtune 4937 based USRP daughterboard.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        @type which: int
        """
        # sets _u and _which
        db_base.db_base.__init__(self, usrp, which)

        self._i2c_addr = (0x60, 0x61)[which]

        self._first_IF = first_IF
        self._second_IF = second_IF
        self._reference_divisor = 640
        self._fast_tuning = False
        self._inverted = False      # FIXME get rid of this
        
        g = self.gain_range()                  # initialize gain
        self.set_gain(float(g[0]+g[1]) / 2)

        self.bypass_adc_buffers(False)
        
    # Gain setting
    def _set_rfagc(self,gain):
        assert gain <= 60 and gain >= 0
        # FIXME this has a 0.5V step between gain = 60 and gain = 59.
        # Why are there two cases instead of a single linear case?
        if gain == 60:
            voltage = 4
        else:
            voltage = gain/60.0 * 2.25 + 1.25
        dacword = int(4096*voltage/1.22/3.3)    # 1.22 = opamp gain

        assert dacword>=0 and dacword<4096
        self._u.write_aux_dac(self._which, 1, dacword)

    def _set_ifagc(self,gain):
        assert gain <= 35 and gain >= 0
        voltage = gain/35.0 * 2.1 + 1.4
        dacword = int(4096*voltage/1.22/3.3)    # 1.22 = opamp gain

        assert dacword>=0 and dacword<4096
        self._u.write_aux_dac(self._which, 0, dacword)

    def _set_pga(self,pga_gain):
        assert pga_gain >=0 and pga_gain <=20
        if(self._which == 0):
            self._u.set_pga (0, pga_gain)
        else:
            self._u.set_pga (2, pga_gain)
            
    def gain_range(self):
        return (0, 115, 1)
    
    def set_gain(self,gain):
        assert gain>=0 and gain<=115
        if gain>60:
            rfgain = 60
            gain = gain - 60
        else:
            rfgain = gain
            gain = 0
        if gain > 35:
            ifgain = 35
            gain = gain - 35
        else:
            ifgain = gain
            gain = 0
        pgagain = gain
        self._set_rfagc(rfgain)
        self._set_ifagc(ifgain)
        self._set_pga(pgagain)
        
    def freq_range(self):
        return (50e6, 860e6, 10e3)

    def set_freq(self, target_freq):
        """
        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        r = self.freq_range()
        if target_freq < r[0] or target_freq > r[1]:
            return (False, 0)
        
        target_lo_freq = target_freq + self._first_IF;  # High side mixing
        f_ref = 4e6 / self._reference_divisor   # frequency steps

        divisor = int((target_lo_freq + (f_ref * 4)) / (f_ref * 8))  
        actual_lo_freq = (f_ref * 8 * divisor)
        actual_freq = actual_lo_freq - self._first_IF;

        if (divisor & ~0x7fff) != 0:		# must be 15-bits or less
            return (False, 0)
        
        # build i2c command string
        buf = [0] * 4
        buf[0] = (divisor >> 8) & 0xff          # DB1
        buf[1] = divisor & 0xff                 # DB2
        buf[2] = control_byte_1(self._fast_tuning, self._reference_divisor)
        buf[3] = control_byte_2(actual_freq, True)  

        ok = self._u.write_i2c(self._i2c_addr, int_seq_to_str (buf))

        return (ok, actual_freq - self._second_IF)

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        return False

    def spectrum_inverted(self):
        """
        The 43.75 MHz version is inverted
        """
        return self._inverted

# hook this daughterboard class into the auto-instantiation framework

# With MT4937DI5-3x7702 with second downconversion
db_instantiator.add(usrp_dbid.TV_RX,
                    lambda usrp, which : (db_tv_rx(usrp, which, 43.75e6, 5.75e6),))

# With MT4937DI5-3x8680, and 3x8769 without second downconversion
db_instantiator.add(usrp_dbid.TV_RX_REV_2,
                    lambda usrp, which : (db_tv_rx(usrp, which, 44e6, 20e6),))

# With MT4937DI5-3x7901 without second downconversion, basically the same as tvrx2
db_instantiator.add(usrp_dbid.TV_RX_REV_3,
                    lambda usrp, which : (db_tv_rx(usrp, which, 44e6, 20e6),))
