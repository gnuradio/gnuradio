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

import math
from usrpm import usrp_dbid
import db_base
import db_instantiator

def int_seq_to_str (seq):
    """convert a sequence of integers into a string"""
    return ''.join (map (chr, seq))

def str_to_int_seq (str):
    """convert a string to a list of integers"""
    return map (ord, str)

class db_dbs_rx (db_base.db_base):
    def __init__ (self, usrp, which):
        """
        Control DBS receiver based USRP daughterboard.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        @type which: int
        """
        # sets _u and _which
        db_base.db_base.__init__(self, usrp, which)

        self._u._write_oe(self._which,0x0001,0x0001)
        self.i2c_addr = (0x67, 0x65)[self._which]
        # set basic parameters
        # set default values
        self.n = 950
        self.div2 = 0
        self.osc = 5
        self.cp = 3
        self.r = 4
        self.r_int = 1
        self.fdac = 127
        self.m = 2
        self.dl = 0
        self.ade = 0
        self.adl = 0
        self.gc2 = 31
        self.diag = 0

        # FIXME this should be in the core dboard class
        self.refclk_divisor = 16
        self._enable_refclk(True)

        g = self.gain_range()
        self.set_gain(float(g[0]+g[1]) / 2)
        self.bypass_adc_buffers(True)
        
    def __del__(self):
        if self._u:
            self._enable_refclk(False)

    def _write_reg (self, regno, v):
        """regno is in [0,5], v is value to write to register"""
        assert (0 <= regno and regno <= 5)
        self._u.write_i2c (self.i2c_addr, int_seq_to_str ((regno, v)))
        
    def _write_regs (self, starting_regno, vals):
        """starting_regno is in [0,5],
        vals is a seq of integers to write to consecutive registers"""
        self._u.write_i2c (self.i2c_addr,
                          int_seq_to_str ((starting_regno,) + tuple (vals)))
        
    def _read_status (self):
        """If successful, return list of two ints: [status_info, filter_DAC]"""
        s = self._u.read_i2c (self.i2c_addr, 2)
        if len (s) != 2:
            return None
        return str_to_int_seq (s)

    def _send_reg(self,regno):
        assert (0 <= regno and regno <= 5)
        if regno == 0:
            self._write_reg(0,(self.div2<<7) + (self.n>>8))
        if regno == 1:
            self._write_reg(1,self.n & 255)
        if regno == 2:
            self._write_reg(2,self.osc + (self.cp<<3) + (self.r_int<<5))
        if regno == 3:
            self._write_reg(3,self.fdac)
        if regno == 4:
            self._write_reg(4,self.m + (self.dl<<5) + (self.ade<<6) + (self.adl<<7))
        if regno == 5:
            self._write_reg(5,self.gc2 + (self.diag<<5))

    # BW setting
    def _set_m(self,m):
        assert m>0 and m<32
        self.m = m
        self._send_reg(4)
    
    def _set_fdac(self,fdac):
        assert fdac>=0 and fdac<128
        self.fdac = fdac
        self._send_reg(3)
        
    def set_bw (self, bw):
        #assert (bw>=4e6 and bw<=33e6)
        assert (bw>=1e6 and bw<=33e6)
        if bw >= 4e6:
            m_max = int(min(31,math.floor(self._refclk_freq()/1e6)))
        elif bw >= 2e6:      # Outside of Specs!
            m_max = int(min(31,math.floor(self._refclk_freq()/.5e6)))
        else:      # Way outside of Specs!
            m_max = int(min(31,math.floor(self._refclk_freq()/.25e6)))
        
        m_min = int(math.ceil(self._refclk_freq()/2.5e6))
        m_test = m_max
        while m_test >= m_min:
            fdac_test = int(round(((bw * m_test / self._refclk_freq())-4)/.145))
            if fdac_test>127:
                m_test = m_test - 1
            else:
                break
        if (m_test>=m_min and fdac_test >=0):
            self._set_m(m_test)
            self._set_fdac(fdac_test)
            return (self.m,self.fdac,self._refclk_freq()/self.m*(4+0.145*self.fdac))
        else:
            print "Failed to set bw"

    # Gain setting
    def _set_dl(self,dl):
        assert dl == 0 or dl == 1
        self.dl = dl
        self._send_reg(4)
        
    def _set_gc2(self,gc2):
        assert gc2<32 and gc2>=0
        self.gc2 = gc2
        self._send_reg(5)

    def _set_gc1(self,gc1):
        assert gc1>=0 and gc1<4096
        self.gc1 = gc1
        self._u.write_aux_dac(self._which,0,int(gc1))

    def _set_pga(self, pga_gain):
        assert pga_gain >=0 and pga_gain <=20
        if(self._which == 0):
            self._u.set_pga (0, pga_gain)
            self._u.set_pga (1, pga_gain)
        else:
            self._u.set_pga (2, pga_gain)
            self._u.set_pga (3, pga_gain)
            
    def gain_range(self):
        return (0, 104, 1)
    
    def set_gain(self,gain):
        if not (gain>=0 and gain<105):
            raise ValueError, "gain out of range"
        gc1 = 0
        gc2 = 0
        dl = 0
        pga = 0
        if gain <56:
            gc1 = int((-gain*1.85/56.0 + 2.6)*4096.0/3.3)
            gain = 0
        else:
            gc1 = 0
            gain = gain - 56
        if gain < 24:
            gc2 = int(round(31.0 * (1-gain/24.0)))
            gain = 0
        else:
            gc2 = 0
            gain = gain - 24
        if gain >= 4.58:
            dl = 1
            gain = gain - 4.58
        pga = gain
        self._set_gc1(gc1)
        self._set_gc2(gc2)
        self._set_dl(dl)
        self._set_pga(pga)
        
    # Frequency setting
    def _set_osc(self,osc):
        assert osc>=0 and osc<8
        self.osc = osc
        self._send_reg(2)

    def _set_cp(self,cp):
        assert cp>=0 and cp<4
        self.cp = cp
        self._send_reg(2)

    def _set_n(self,n):
        assert n>256 and n<32768
        self.n = n
        self._send_reg(0)
        self._send_reg(1)

    def _set_div2(self,div2):
        assert div2 == 0 or div2 == 1
        self.div2 = div2
        self._send_reg(0)

    def _set_r(self,r):
        assert r>=0 and r<128
        self.r = r
        self.r_int = int(round(math.log10(r)/math.log10(2)) - 1)
        self._send_reg(2)
        
    # FIXME  How do we handle ADE and ADL properly?
    def _set_ade(self,ade):
        assert ade == 0 or ade == 1
        self.ade = ade
        self._send_reg(4)
        
    def freq_range(self):
        return (500e6, 2.6e9, 1e6)

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
    def set_freq(self, freq):
        """
        Set the frequency.

        @param freq:  target frequency in Hz
        @type freq:   float

        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        if not (freq>=500e6 and freq<=2.6e9):
            return (False, 0)
        
        if(freq<1150e6):
            self._set_div2(0)
            vcofreq = 4 * freq
        else:
            self._set_div2(1)
            vcofreq = 2 * freq
        self._set_ade(1)
        rmin=max(2,self._refclk_freq()/2e6)
        rmax=min(128,self._refclk_freq()/500e3)
        r = 2
        n=0
        best_r = 2
        best_n =0
        best_delta = 10e6
        while r <= rmax:
            n = round(freq/(self._refclk_freq()/r))
            if r<rmin or n<256:
                r = r * 2
                continue
            delta = abs(n*self._refclk_freq()/r - freq)
            if delta < 75e3:
                best_r = r
                best_n = n
                break
            if delta < best_delta*0.9:
                best_r = r
                best_n = n
                best_delta = delta
            r = r * 2
        self._set_r(int(best_r))

        self._set_n(int(round(best_n)))
 
        if vcofreq < 2433e6:
            vco = 0
        elif vcofreq < 2711e6:
            vco=1
        elif vcofreq < 3025e6:
            vco=2
        elif vcofreq < 3341e6:
            vco=3
        elif vcofreq < 3727e6:
            vco=4
        elif vcofreq < 4143e6:
            vco=5
        elif vcofreq < 4493e6:
            vco=6
        else:
            vco=7

        self._set_osc(vco)

        # Set CP current
        adc_val = 0
        while adc_val == 0 or adc_val == 7:
            (byte1,byte2) = self._read_status()
            adc_val = byte1 >> 2
            if(adc_val == 0):
                if vco <= 0:
                    return (False, 0)
                else:
                    vco = vco - 1
            elif(adc_val == 7):
                if(vco >= 7):
                    return (False, 0)
                else:
                    vco = vco + 1
            self._set_osc(vco)
        if adc_val == 1 or adc_val == 2:
            self._set_cp(1)
        elif adc_val == 3 or adc_val == 4:
            self._set_cp(2)
        else:
            self._set_cp(3)
                    
        return (True, self.n * self._refclk_freq() / self.r)

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        return True

# hook this daughterboard class into the auto-instantiation framework
db_instantiator.add(usrp_dbid.DBS_RX, lambda usrp, which : (db_dbs_rx(usrp, which),))
