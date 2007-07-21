#
# Copyright 2003,2004 Free Software Foundation, Inc.
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

from gnuradio import gr

class sdr_1000 (gr.sdr_1000_base):
    "Control the DDS on the SDR-1000"
    def __init__(self, pport = 0):
	gr.sdr_1000_base.__init__(self, pport)
        self.write_latch (3, 0x00, 0xC0) # Reset low, WRS/ low
        self.write_reg (0x20, 0x40)

    def write_reg(self, addr, data):
        self.write_latch (3, addr & 0x3f, 0x3f)
        self.write_latch (2, data, 0xff)
        self.write_latch (3, 0x40, 0x40)
        self.write_latch (3, 0x00, 0x40)
    
    def set_freq(self, freq):
        self.set_band (freq)
        ftw = freq / 200e6;
        for i in xrange(6):
            word = int(ftw * 256)
            ftw = ftw*256 - word
            # print (('%d [%02x]') % (i, word))
            self.write_reg (4+i, word)

    def set_band (self, freq):
        if freq <= 2.25e6:
            band = 0
        elif freq <= 5.5e6:
            band = 1
        elif freq <= 11e6:
            band = 3	# due to wiring mistake on board
        elif freq <= 22e6:
            band = 2	# due to wiring mistake on board
        elif freq <= 37.5e6:
            band = 4
        else:
            band = 5
        
        self.write_latch (1, 1 << band, 0x3f)

    def set_bit (self, reg, bit, state):
        val = 0x00
        if state: val = 1<<bit
        self.write_latch (reg, val, 1<<bit)
        
    def set_tx (self, on = 1):
        self.set_bit(1, 6, on)

    def set_rx (self):
        self.set_bit(1, 6, 0)
    
    def set_gain (self, high):
        self.set_bit(0, 7, high)
      
    def set_mute (self, mute = 1):
        self.set_bit(1, 7, mute)

    def set_unmute (self):
        self.set_bit(1, 7, 0)

    def set_external_pin (self, pin, on = 1):
        assert (pin < 8 and pin > 0), "Out of range 1..7"
        self.set_bit(0, pin-1, on)
