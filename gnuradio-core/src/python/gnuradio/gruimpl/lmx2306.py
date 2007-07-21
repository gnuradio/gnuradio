#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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

'''Control National LMX2306 based frequency synthesizer'''

from gnuradio import gr
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

# bottom two bits of 21 bit word select which register to program

R_REG =  0x0
AB_REG = 0x1
F_REG  = 0x2

F_counter_reset = (1 << 2)
F_phase_detector_polarity = (1 << 7)

F_LD_tri_state           = (0 << 4)
F_LD_R_divider_output    = (4 << 4)
F_LD_N_divider_output    = (2 << 4)
F_LD_serial_data_output  = (6 << 4)
F_LD_digital_lock_detect = (1 << 4)
F_LD_open_drain          = (5 << 4)
F_LD_high                = (3 << 4)
F_LD_low                 = (7 << 4)

# F_default = F_LD_digital_lock_detect | F_phase_detector_polarity
F_default = F_LD_open_drain | F_phase_detector_polarity

#
# 4 control pins:
#   CE   always high
#   LE   load enable.  When LE goes high, data stored in the shift register
#        is loaded into one of the three registers
#   CLK  data is clocked in on the rising edge
#   DATA single data bit.  Entered MSB first

DB_CLK =  (1 << 0)
DB_DATA = (1 << 1)
DB_LE =   (1 << 2)
DB_CE =   (1 << 3)

class lmx2306 (object):
    '''Control the National LMX2306 PLL'''
    __slots__ = ['pp', 'shadow', 'fosc', 'r', 'step_size', 'verbose']
    def __init__ (self, fosc, step_size, which_pp = 0):
        '''FOSC is the frequency of the reference oscillator,
        STEP_SIZE is the step between valid frequencies,
        WHICH_PP specifies which parallel port to use
        '''
        self.pp = gr.make_ppio (which_pp)
        self.shadow = DB_CE
        self.pp.lock ()
        self.pp.write_data (self.shadow)
        self.pp.unlock ()
        self.verbose = False
        self._set_fosc (fosc)
        self._set_step (step_size)

        
    def program (self, r, a, b):
        if self.verbose:
            print "lmx2306: r = %d  a = %d  b = %d" % (r, a, b)
        self.pp.lock ()
        self._write_word (F_REG | F_default | F_counter_reset)
        self._write_word (R_REG | ((r & 0x3fff) << 2))
        self._write_word (AB_REG | ((a & 0x1f) << 2) | ((b & 0x1fff) << 7))
        self._write_word (F_REG | F_default)
        self.pp.unlock ()

    def set_freq (self, freq):
        '''Set the PLL frequency to FREQ

        Return the actual freq value set.  It will be rounded down to a
        multiple of step_size
        '''
        divisor = int (freq / self.step_size)
        actual = divisor * self.step_size
        (a, b) = self._compute_ab (divisor)
        self.program (self.r, a, b)
        return actual

    # ----------------------------------------------------------------
    
    def _set_fosc (self, ref_oscillator_freq):
        self.fosc = ref_oscillator_freq
        
    def _set_step (self, step_size):
        r = int (self.fosc / step_size)
        if r * step_size != self.fosc:
            raise ValueError, "step_size is not a factor of self.fosc"
        if r < 3 or r > 16383:
            raise ValueError, "r is out of range"
        self.r = r
        self.step_size = step_size
        
    def _compute_ab (self, divisor):
        b = divisor / 8
        a = divisor - (b * 8)
        if b < 3 or b > 8191 or a > b:
            raise ValueError, "Invalid divisor"
        return (a, b)

    def _write_word (self, w):
        for i in range(21):
            if w & (1 << 20):
                self._set_DATA_1 ()
            else:
                self._set_DATA_0 ()
            w = (w << 1) & 0x0ffffff
            self._set_CLK_1 ()
            self._set_CLK_0 ()
        self._set_LE_1 ()
        self._set_LE_0 ()

    def _set_LE_0 (self):
        self.shadow = self.shadow & ~DB_LE
        self.pp.write_data (self.shadow)

    def _set_LE_1 (self):
        self.shadow = self.shadow | DB_LE
        self.pp.write_data (self.shadow)

    def _set_CLK_0 (self):
        self.shadow = self.shadow & ~DB_CLK
        self.pp.write_data (self.shadow)

    def _set_CLK_1 (self):
        self.shadow = self.shadow | DB_CLK
        self.pp.write_data (self.shadow)

    def _set_DATA_0 (self):
        self.shadow = self.shadow & ~DB_DATA
        self.pp.write_data (self.shadow)

    def _set_DATA_1 (self):
        self.shadow = self.shadow | DB_DATA
        self.pp.write_data (self.shadow)

if __name__ == '__main__':
    parser = OptionParser (option_class=eng_option)
    parser.add_option ("-o", "--fosc", type="eng_float", default=32e6,
                       help="set reference oscillator freq to FREQ", metavar="FREQ")
    parser.add_option ("-s", "--step-size", type="eng_float", default=10e3,
                       help="set the frequency step size to STEP_SIZE")
    parser.add_option ("-f", "--freq", type="eng_float", default=430e6,
                       help="set VCO frequency to FREQ")
    parser.add_option ("-v", "--verbose", action="store_true", default=False)
    (options, args) = parser.parse_args ()

    if options.verbose:
        print "fosc = %s  step = %s  fvco = %s" % (
            eng_notation.num_to_str (options.fosc),
            eng_notation.num_to_str (options.step_size),
            eng_notation.num_to_str (options.freq))
    
    lmx = lmx2306 (options.fosc, options.step_size)
    lmx.verbose = options.verbose

    actual = lmx.set_freq (options.freq)

    if options.verbose:
        print "fvco_actual = %s  delta = %s" % (
            eng_notation.num_to_str (actual),
            eng_notation.num_to_str (options.freq - actual))
