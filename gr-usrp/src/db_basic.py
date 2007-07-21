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

import sys
from usrpm import usrp_dbid
import db_base
import db_instantiator

class db_basic_tx(db_base.db_base):
    def __init__(self, usrp, which):
        """
        Handler for Basic Tx daughterboards.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to TX_A or TX_B respectively
        """
        # sets _u and _which
        db_base.db_base.__init__(self, usrp, which)

        if 0:        # Doing this would give us a different default than the historical values...
            g = self.gain_range()                  # initialize gain
            self.set_gain(float(g[0]+g[1]) / 2)


    def freq_range(self):
        """
        Return range of frequencies in Hz that can be tuned by this d'board.

        @returns (min_freq, max_freq, step_size)
        @rtype tuple

        We say we can do pretty much anything...
        """
        return (-90e9, 90e9, 1e-6)

    def set_freq(self, target_freq):
        """
        Set the frequency.

        @param freq:  target RF frequency in Hz
        @type freq:   float

        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        return (True, 0)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.

        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max(), self._u.pga_db_per_step())

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        ok = self._u.set_pga(self._which * 2 + 0, gain)
        ok = ok and self._u.set_pga(self._which * 2 + 1, gain)
        return ok

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.
        """
        return True
    

class db_basic_rx(db_base.db_base):
    def __init__(self, usrp, which, subdev):
        """
        Handler for Basic Rx daughterboards.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        @param subdev: which analog i/o channel: 0 or 1
        @type subdev: int
        """
        # sets _u and _which
        db_base.db_base.__init__(self, usrp, which)
        self._subdev = subdev

        self.bypass_adc_buffers(True)

        if 0:        # Doing this would give us a different default than the historical values...
            g = self.gain_range()                  # initialize gain
            self.set_gain(float(g[0]+g[1]) / 2)


    def freq_range(self):
        """
        Return range of frequencies in Hz that can be tuned by this d'board.

        @returns (min_freq, max_freq, step_size)
        @rtype tuple

        We say we can do pretty much anything...
        """
        return (0, 90e9, 1e-6)

    def set_freq(self, target_freq):
        """
        Set the frequency.

        @param freq:  target RF frequency in Hz
        @type freq:   float

        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        return (True, 0)
        

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.

        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max(), self._u.pga_db_per_step())

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        return self._u.set_pga(self._which * 2 + self._subdev, gain)

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        return False
    
class db_lf_rx(db_basic_rx):
    def __init__(self, usrp, which, subdev):
        """
        Handler for Low Freq Rx daughterboards.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        @param subdev: which analog i/o channel: 0 or 1
        @type subdev: int
        """
        # sets _u and _which
        db_basic_rx.__init__(self, usrp, which, subdev)

    def freq_range(self):
        """
        Return range of frequencies in Hz that can be tuned by this d'board.

        @returns (min_freq, max_freq, step_size)
        @rtype tuple

        We cover the first nyquist zone only
        """
        return (0, 32e6, 1e-6)

class db_lf_tx(db_basic_tx):
    def __init__(self, usrp, which):
        """
        Handler for Low Freq Tx daughterboards.
        
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to RX_A or RX_B respectively
        """
        # sets _u and _which
        db_basic_tx.__init__(self, usrp, which)

    def freq_range(self):
        """
        Return range of frequencies in Hz that can be tuned by this d'board.

        @returns (min_freq, max_freq, step_size)
        @rtype tuple

        We cover the first nyquist zone only
        """
        return (-32e6, 32e6, 1e-6)


# hook these daughterboard classes into the auto-instantiation framework

def _basic_rx_instantiator(usrp, which):
    # two single channel subdevices
    return (db_basic_rx(usrp, which, 0), db_basic_rx(usrp, which, 1))

def _lf_rx_instantiator(usrp, which):
    # two single channel subdevices
    return (db_lf_rx(usrp, which, 0), db_lf_rx(usrp, which, 1))

def _basic_tx_instantiator(usrp, which):
    # one quadrature subdevice
    return (db_basic_tx(usrp, which),)

def _lf_tx_instantiator(usrp, which):
    # one quadrature subdevice
    return (db_lf_tx(usrp, which),)

def _no_db_instantiator(usrp, which):
    if hasattr(usrp, 'tx_freq'):   # is this a tx or rx daughterboard?
        return (_basic_tx_instantiator(usrp, which))
    else:
        return (_basic_rx_instantiator(usrp, which))
    
def _invalid_instantiator(usrp, which):
    if hasattr(usrp, 'tx_freq'):   # is this a tx or rx daughterboard?
        sys.stderr.write('\n\aWarning: Treating daughterboard with invalid EEPROM contents as if it were a "Basic Tx."\n')
        sys.stderr.write('Warning: This is almost certainly wrong...  Use appropriate burn-*-eeprom utility.\n\n')
        return _basic_tx_instantiator(usrp, which)
    else:
        sys.stderr.write('\n\aWarning: Treating daughterboard with invalid EEPROM contents as if it were a "Basic Rx."\n')
        sys.stderr.write('Warning: This is almost certainly wrong...  Use appropriate burn-*-eeprom utility.\n\n')
        return _basic_rx_instantiator(usrp, which)

db_instantiator.add(-1, _no_db_instantiator)                  # no daughterboard
db_instantiator.add(-2, _invalid_instantiator)                # invalid eeprom contents
db_instantiator.add(usrp_dbid.BASIC_TX, _basic_tx_instantiator)
db_instantiator.add(usrp_dbid.BASIC_RX, _basic_rx_instantiator)
db_instantiator.add(usrp_dbid.LF_TX, _lf_tx_instantiator)
db_instantiator.add(usrp_dbid.LF_RX, _lf_rx_instantiator)
