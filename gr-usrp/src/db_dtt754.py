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

def control_byte_1():
    RS = 0  # 0 = 166.66kHz reference
    ATP = 7  # Disable internal AGC
    return 0x80 | ATP<<3 | RS

def control_byte_2():
    STBY = 0  # powered on
    XTO = 1  # turn off xtal out, which we don't have
    ATC = 0  # not clear exactly, possibly speeds up or slows down AGC, which we are not using
    
    c = 0xc2 | ATC<<5 | STBY<<4 | XTO
    return c

def bandswitch_byte(freq,bw):
    if(bw>7.5e6):
        P5 = 1
    else:
        P5 = 0

    if freq < 121e6:
        CP = 0
        BS = 1
    elif freq < 141e6:
        CP = 1
        BS = 1
    elif freq < 166e6:
        CP = 2
        BS = 1
    elif freq < 182e6:
        CP = 3
        BS = 1
    elif freq < 286e6:
        CP = 0
        BS = 2
    elif freq < 386e6:
        CP = 1
        BS = 2
    elif freq < 446e6:
        CP = 2
        BS = 2
    elif freq < 466e6:
        CP = 3
        BS = 2
    elif freq < 506e6:
        CP = 0
        BS = 8
    elif freq < 761e6:
        CP = 1
        BS = 8
    elif freq < 846e6:
        CP = 2
        BS = 8
    else:  # limit is ~905 MHz
        CP = 3
        BS = 8
    return CP<<6 | P5 << 4 | BS

class db_dtt754(db_base.db_base):
    def __init__(self, usrp, which):
        """
        Control custom DTT75403-based daughterboard.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        @type which: int
        """
        # sets _u and _which
        db_base.db_base.__init__(self, usrp, which)

        self._i2c_addr = (0x60, 0x62)[which]
        self.bw = 7e6
        self._IF = 36e6
        
        self.f_ref = 166.6666e3
        self._inverted = False  
        
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
        return (44e6, 900e6, 10e3)

    def set_freq(self, target_freq):
        """
        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        r = self.freq_range()
        if target_freq < r[0] or target_freq > r[1]:
            return (False, 0)
        
        target_lo_freq = target_freq + self._IF;  # High side mixing

        divisor = int(0.5+(target_lo_freq / self.f_ref))
        actual_lo_freq = self.f_ref*divisor

        if (divisor & ~0x7fff) != 0:		# must be 15-bits or less
            return (False, 0)
        
        # build i2c command string
        buf = [0] * 5
        buf[0] = (divisor >> 8) & 0xff          # DB1
        buf[1] = divisor & 0xff                 # DB2
        buf[2] = control_byte_1()
        buf[3] = bandswitch_byte(actual_lo_freq,self.bw)
        buf[4] = control_byte_2()  

        ok = self._u.write_i2c(self._i2c_addr, int_seq_to_str (buf))

        self.freq = actual_lo_freq - self._IF
        
        return (ok, actual_lo_freq)

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

    def set_bw(self,bw):
        """
        Choose the SAW filter bandwidth, either 7MHz or 8MHz)
        """
        self.bw = bw
        self.set_freq(self.freq)
        
# hook this daughterboard class into the auto-instantiation framework

# With DTT75403
db_instantiator.add(usrp_dbid.DTT754,
                    lambda usrp, which : (db_dtt754(usrp, which),))
