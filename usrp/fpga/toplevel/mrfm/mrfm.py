#!/usr/bin/env python
#
# This is mrfm_fft_sos.py
# Modification of Matt's mrfm_fft.py that reads filter coefs from file
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import usrp

class source_c(usrp.source_c):
    def __init__(self,fpga_filename):
        usrp.source_c.__init__(self,which=0, decim_rate=64, nchan=2, mux=0x32103210, mode=0,
                               fpga_filename=fpga_filename)

        self._write_9862(0,2,0x80)  # Bypass ADC buffer, minimum gain
        self._write_9862(0,3,0x80)  # Bypass ADC buffer, minimum gain

        self._write_9862(0,8,0)   # TX PWR Down
        self._write_9862(0,10,0)  # DAC offset
        self._write_9862(0,11,0)  # DAC offset
        self._write_9862(0,14,0x80)  # gain
        self._write_9862(0,16,0xff)  # pga
        self._write_9862(0,18,0x0c)  # TX IF
        self._write_9862(0,19,0x01)  # TX Digital
        self._write_9862(0,20,0x00)  # TX Mod

        # max/min values are +/-2, so scale is set to make 2 = 32767 

        self._write_fpga_reg(69,0x0e)   # debug mux
        self._write_fpga_reg(5,-1)
        self._write_fpga_reg(7,-1)
        self._write_oe(0,0xffff, 0xffff)
        self._write_oe(1,0xffff, 0xffff)
        self._write_fpga_reg(14,0xf)

        self.decim = None
        
    def set_coeffs(self,frac_bits,b20,b10,b00,a20,a10,b21,b11,b01,a21,a11):
        def make_val(address,value):
            return (address << 16) | (value & 0xffff)

        # gain, scale already included in a's and b's from file

        self._write_fpga_reg(67,make_val(1,b20))
        self._write_fpga_reg(67,make_val(2,b10))
        self._write_fpga_reg(67,make_val(3,b00))
        self._write_fpga_reg(67,make_val(4,a20))
        self._write_fpga_reg(67,make_val(5,a10))
        
        self._write_fpga_reg(67,make_val(7,b21))
        self._write_fpga_reg(67,make_val(8,b11))
        self._write_fpga_reg(67,make_val(9,b01))
        self._write_fpga_reg(67,make_val(10,a21))
        self._write_fpga_reg(67,make_val(11,a11))
        
        self._write_fpga_reg(68,frac_bits)   # Shift
        
        print "Biquad 0 : b2=%d b1=%d b0=%d a2=%d a1=%d" % (b20,b10,b00,a20,a10)
        print "Biquad 1 : b2=%d b1=%d b0=%d a2=%d a1=%d" % (b21,b11,b01,a21,a11)

    def set_decim_rate(self,rate=None):
        i=2
        turn=1
        a=1
        b=1
        while (rate>1) and (i<257):
            if (rate/i) * i == rate:
                if turn == 1:
                    if a*i<257:
                        a = a * i
                        turn = 0
                    elif b*i<257:
                        b = b * i
                        turn = 0
                    else:
                        print "Failed to set DECIMATOR"
                        return self.decim
                elif b*i<257:
                    b = b * i
                    turn = 1
                elif a*i<257:
                    a = a * i
                    turn = 1
                else:
                    print "Failed to set DECIMATOR"
                    return self.decim
                rate=rate/i
                continue
            i = i + 1
        if rate > 1:
            print "Failed to set DECIMATOR"
            return self.decim
        else:    
            self.decim = a*b
            print "a = %d  b = %d" % (a,b)
            self._write_fpga_reg(64,(a-1)*256+(b-1))   # Set actual decimation

    def decim_rate(self):
        return self.decim
    
    def set_center_freq(self,freq):
        self._write_fpga_reg(65,int(-freq/64e6*65536*65536))  # set center freq

    def set_compensator(self,a11,a12,a21,a22,shift):
        self._write_fpga_reg(70,a11)
        self._write_fpga_reg(71,a12)
        self._write_fpga_reg(72,a21)
        self._write_fpga_reg(73,a22)
        self._write_fpga_reg(74,shift)   # comp shift
        
