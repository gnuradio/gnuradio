#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

import weakref
from usrpm import usrp_prims
from usrpm.usrp_fpga_regs import *

class db_base(object):
    """
    Abstract base class for all daughterboards.

    This defines the required operations and interfaces for all d'boards.
    """
    def __init__(self, usrp, which):
        """
        Initialize daughterboard interface.

        @param usrp: instance of usrp
        @param which: which daughterboard side: A = 0, B = 1
        @type which: int
        """

        if not (which in (0, 1)):
            raise ValueError, "Invalid value of which: %s" % (which,)

        self._u = weakref.proxy(usrp)

        self._which = which
        if hasattr(self._u, 'tx_freq'):   # is this a tx or rx daughterboard?
            self._tx = True
            self._slot = which * 2
        else:
            self._tx = False
            self._slot = which * 2 + 1

        self._refclk_reg = (FR_TX_A_REFCLK,FR_RX_A_REFCLK,FR_TX_B_REFCLK,FR_RX_B_REFCLK)[self._slot]

    def dbid(self):
        return self._u.daughterboard_id(self._which)

    def name(self):
        return usrp_prims.usrp_dbid_to_string(self.dbid())

    def side_and_name(self):
        return "AB"[self._which] + ': ' + self.name()

    # Function to bypass ADC buffers.  Any board which is DC-coupled should bypass the buffers
    def bypass_adc_buffers(self,bypass):
        if self._tx:
            raise RuntimeError, "TX Board has no adc buffers"
        if self._which==0:
            self._u.set_adc_buffer_bypass(0, bypass)
            self._u.set_adc_buffer_bypass(1, bypass)
        else:
            self._u.set_adc_buffer_bypass(2, bypass)
            self._u.set_adc_buffer_bypass(3, bypass)
        
    # ------------------------------------------------------------------------
    # Reference Clock section

    # Control whether a reference clock is sent to the daughterboards,
    # and what frequency
    #
    # Bit 7  -- 1 turns on refclk, 0 allows IO use
    # Bits 6:0 Divider value
    #
    
    def _refclk_freq(self):
        return self._u.fpga_master_clock_freq()/self._refclk_divisor()
    
    def _enable_refclk(self,enable):
        CLOCK_OUT = 1   # Clock is on lowest bit
        REFCLK_ENABLE = 0x80
        REFCLK_DIVISOR_MASK = 0x7f
        if enable:
            self._u._write_oe(self._which, CLOCK_OUT, CLOCK_OUT)  # output enable
            self._u._write_fpga_reg(self._refclk_reg,
                                   ((self._refclk_divisor() & REFCLK_DIVISOR_MASK)
                                    | REFCLK_ENABLE))
        else:
            self._u._write_fpga_reg(self._refclk_reg, 0)
            
    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        raise NotImplementedError

    # ------------------------------------------------------------------------
    # Automatic Transmit/Receive switching
    #
    # The presence or absence of data in the FPGA transmit fifo
    # selects between two sets of values for each of the 4 banks of
    # daughterboard i/o pins.
    #
    # Each daughterboard slot has 3 16-bit registers associated with it:
    #   FR_ATR_MASK_*, FR_ATR_TXVAL_* and FR_ATR_RXVAL_*
    #
    # FR_ATR_MASK_{0,1,2,3}: 
    #
    #   These registers determine which of the daugherboard i/o pins are
    #   affected by ATR switching.  If a bit in the mask is set, the
    #   corresponding i/o bit is controlled by ATR, else it's output
    #   value comes from the normal i/o pin output register:
    #   FR_IO_{0,1,2,3}.
    #
    # FR_ATR_TXVAL_{0,1,2,3}:
    # FR_ATR_RXVAL_{0,1,2,3}:
    #
    #   If the Tx fifo contains data, then the bits from TXVAL that are
    #   selected by MASK are output.  Otherwise, the bits from RXVAL that
    #   are selected by MASK are output.

    def set_atr_mask(self, v):
        """
        Set Auto T/R mask.
        """
        return self._u._write_fpga_reg(FR_ATR_MASK_0 + 3 * self._slot, v)

    def set_atr_txval(self, v):
        """
        Set Auto T/R register value to be used when transmitting.
        """
        return self._u._write_fpga_reg(FR_ATR_TXVAL_0 + 3 * self._slot, v)

    def set_atr_rxval(self, v):
        """
        Set Auto T/R register value to be used when receiving.
        """
        return self._u._write_fpga_reg(FR_ATR_RXVAL_0 + 3 * self._slot, v)

    def set_atr_tx_delay(self, v):
	"""
	Set Auto T/R delay (in clock ticks) from when Tx fifo gets data to 
	when T/R switches.
	"""
	return self._u._write_fpga_reg(FR_ATR_TX_DELAY, v)
	
    def set_atr_rx_delay(self, v):
	"""
	Set Auto T/R delay (in clock ticks) from when Tx fifo goes empty to 
	when T/R switches.
	"""
	return self._u._write_fpga_reg(FR_ATR_RX_DELAY, v)
	
    # derived classes should override the following methods

    def freq_range(self):
        """
        Return range of frequencies in Hz that can be tuned by this d'board.

        @returns (min_freq, max_freq, step_size)
        @rtype tuple
        """
        raise NotImplementedError

    def set_freq(self, target_freq):
        """
        Set the frequency.

        @param freq:  target RF frequency in Hz
        @type freq:   float

        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        raise NotImplementedError

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.

        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        raise NotImplementedError

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        raise NotImplementedError

    def is_quadrature(self):
        """
        Return True if this daughterboard does quadrature up or down conversion.
        That is, return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        raise NotImplementedError

    def i_and_q_swapped(self):
        """
        Return True if this is a quadrature device and ADC 0 is Q.
        """
        return False

    def spectrum_inverted(self):
        """
        Return True if the dboard gives an inverted spectrum
        """
        return False
    
    def set_enable(self, on):
        """
        For tx daughterboards, this controls the transmitter enable.
        """
        pass
    
    def set_auto_tr(self,on):
        """
        Enable automatic Transmit/Receive switching (ATR).

        Should be overridden in subclasses that care.  This will typically
        set the atr_mask, txval and rxval.
        """
        pass

    def set_lo_offset(self, offset):
	"""
	Set how much LO is offset from requested frequency
	
	Should be overriden by daughterboards that care.
	"""
	pass
	
    def lo_offset(self, offset):
	"""
	Get how much LO is offset from requested frequency

	Should be overriden by daughterboards that care.
	"""
	return 0.0

    def select_rx_antenna(self, which_antenna):
	"""
	Specify which antenna port to use for reception.
	Should be overriden by daughterboards that care.
	"""
	pass
