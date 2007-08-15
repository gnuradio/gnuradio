#
# Copyright 2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import usrp1
import time,math

from usrpm import usrp_dbid
import db_base
import db_instantiator
from usrpm.usrp_fpga_regs import *

#debug_using_gui = True                  # Must be set to True or False
debug_using_gui = False                  # Must be set to True or False

#if debug_using_gui:
#    import flexrf_debug_gui

# d'board i/o pin defs

# TX IO Pins
TX_POWER = (1 << 0)         # TX Side Power
RX_TXN = (1 << 1)           # T/R antenna switch for TX/RX port

# RX IO Pins
RX2_RX1N = (1 << 0)         # antenna switch between RX2 and TX/RX port
RXENABLE = (1 << 1)         # enables mixer
PLL_LOCK_DETECT = (1 << 2)  # Muxout pin from PLL -- MUST BE INPUT
MReset = (1 << 3)           # NB6L239 Master Reset, asserted low
SELA0 = (1 << 4)            # NB6L239 SelA0
SELA1 = (1 << 5)            # NB6L239 SelA1
SELB0 = (1 << 6)            # NB6L239 SelB0
SELB1 = (1 << 7)            # NB6L239 SelB1
PLL_ENABLE = (1 << 8)       # CE Pin on PLL
AUX_SCLK = (1 << 9)         # ALT SPI SCLK
AUX_SDO = (1 << 10)         # ALT SPI SDO
AUX_SEN = (1 << 11)         # ALT SPI SEN

SPI_ENABLE_TX_A = usrp1.SPI_ENABLE_TX_A
SPI_ENABLE_TX_B = usrp1.SPI_ENABLE_TX_B
SPI_ENABLE_RX_A = usrp1.SPI_ENABLE_RX_A
SPI_ENABLE_RX_B = usrp1.SPI_ENABLE_RX_B


"""
A few comments about the WBX boards:
  They are half-duplex.  I.e., transmit and receive are mutually exclusive.
  There is a single LO for both the Tx and Rx sides.
  The the shared control signals are hung off of the Rx side.
  The shared io controls are duplexed onto the Rx side pins.
  The wbx_high d'board always needs to be in 'auto_tr_mode'
"""


class wbx_base(db_base.db_base):
    """
    Abstract base class for all wbx boards.

    Derive board specific subclasses from db_wbx_base_{tx,rx}
    """
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to side A or B respectively
        @type which: int
        """
        # sets _u  _which _tx and _slot
        db_base.db_base.__init__(self, usrp, which)

        self.first = True
        self.spi_format = usrp1.SPI_FMT_MSB | usrp1.SPI_FMT_HDR_0

        # FIXME -- the write reg functions don't work with 0xffff for masks
        self._rx_write_oe(int(PLL_ENABLE|MReset|SELA0|SELA1|SELB0|SELB1|RX2_RX1N|RXENABLE), 0x7fff)
        self._rx_write_io((PLL_ENABLE|MReset|0|RXENABLE), (PLL_ENABLE|MReset|RX2_RX1N|RXENABLE))

        self._tx_write_oe((TX_POWER|RX_TXN), 0x7fff)
        self._tx_write_io((0|RX_TXN), (TX_POWER|RX_TXN))  # TX off, TR switch set to RX

        self.spi_enable = (SPI_ENABLE_RX_A, SPI_ENABLE_RX_B)[which]

        self.set_auto_tr(False)
        
        #if debug_using_gui:
        #    title = "FlexRF Debug Rx"
        #    if self._tx:
        #        title = "FlexRF Debug Tx"
        #    self.gui = flexrf_debug_gui.flexrf_debug_gui(self, title)
        #    self.gui.Show(True)


    def __del__(self):
        #self._u.write_io(self._which, self.power_off, POWER_UP)   # turn off power to board
        #self._u._write_oe(self._which, 0, 0xffff)   # turn off all outputs
        self.set_auto_tr(False)

    def _lock_detect(self):
        """
        @returns: the value of the VCO/PLL lock detect bit.
        @rtype: 0 or 1
        """
        if self._rx_read_io() & PLL_LOCK_DETECT:
            return True
        else:      # Give it a second chance
            if self._rx_read_io() & PLL_LOCK_DETECT:
                return True
            else:
                return False
        
    # Both sides need access to the Rx pins.
    # Write them directly, bypassing the convenience routines.
    # (Sort of breaks modularity, but will work...)

    def _tx_write_oe(self, value, mask):
        return self._u._write_fpga_reg((FR_OE_0, FR_OE_2)[self._which],
                                       ((mask & 0xffff) << 16) | (value & 0xffff))

    def _rx_write_oe(self, value, mask):
        return self._u._write_fpga_reg((FR_OE_1, FR_OE_3)[self._which],
                                       ((mask & 0xffff) << 16) | (value & 0xffff))

    def _tx_write_io(self, value, mask):
        return self._u._write_fpga_reg((FR_IO_0, FR_IO_2)[self._which],
                                       ((mask & 0xffff) << 16) | (value & 0xffff))

    def _rx_write_io(self, value, mask):
        return self._u._write_fpga_reg((FR_IO_1, FR_IO_3)[self._which],
                                       ((mask & 0xffff) << 16) | (value & 0xffff))

    def _rx_read_io(self):
        t = self._u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self._which])
        return (t >> 16) & 0xffff

    def _tx_read_io(self):
        t = self._u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self._which])
        return t & 0xffff


    def _compute_regs(self, freq):
        """
        Determine values of registers, along with actual freq.
        
        @param freq: target frequency in Hz
        @type freq: float
        @returns: (R, N, func, init, actual_freq)
        @rtype: tuple(int, int, int, int, float)
        
        Override this in derived classes.
        """
        raise NotImplementedError

    def _refclk_freq(self):
        return float(self._u.fpga_master_clock_freq())/self._refclk_divisor()

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
    # ----------------------------------------------------------------
    
    def set_freq(self, freq):
        """
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

    def _set_pga(self, pga_gain):
        if(self._which == 0):
            self._u.set_pga (0, pga_gain)
            self._u.set_pga (1, pga_gain)
        else:
            self._u.set_pga (2, pga_gain)
            self._u.set_pga (3, pga_gain)

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        return True

# ----------------------------------------------------------------

class wbx_base_tx(wbx_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.sink_c
        @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
        """
        wbx_base.__init__(self, usrp, which)

        # power up the transmit side, NO -- but set antenna to receive
        self._u.write_io(self._which, (TX_POWER), (TX_POWER|RX_TXN))
        self._lo_offset = 0e6

        #  Gain is not set by the PGA, but the PGA must be set at max gain in the TX
        return self._set_pga(self._u.pga_max())

    def __del__(self):
        # Power down and leave the T/R switch in the R position
        self._u.write_io(self._which, (RX_TXN), (TX_POWER|RX_TXN))
        wbx_base.__del__(self)

    def set_auto_tr(self, on):
        if on:
            self.set_atr_mask (RX_TXN)
            self.set_atr_txval(0)
            self.set_atr_rxval(RX_TXN)
        else:
            self.set_atr_mask (0)
            self.set_atr_txval(0)
            self.set_atr_rxval(0)

    def set_enable(self, on):
        """
        Enable transmitter if on is True
        """
        if on:
            v = 0
        else:
            v = RX_TXN
        self._u.write_io(self._which, v, RX_TXN)

    def set_lo_offset(self, offset):
	"""
	Set amount by which LO is offset from requested tuning frequency.
	
	@param offset: offset in Hz
	"""
	self._lo_offset = offset

    def lo_offset(self):
	"""
	Get amount by which LO is offset from requested tuning frequency.
	
	@returns Offset in Hz
	"""
	return self._lo_offset
	
class wbx_base_rx(wbx_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.source_c
        @param which: 0 or 1 corresponding to side RX_A or RX_B respectively.
        """
        wbx_base.__init__(self, usrp, which)
        
        # set up for RX on TX/RX port
        self.select_rx_antenna('TX/RX')

        self.bypass_adc_buffers(True)

        self._lo_offset = -4e6

    def __del__(self):
        # Power down
        self._u.write_io(self._which, 0, (RXENABLE))
        wbx_base.__del__(self)
    
    def set_auto_tr(self, on):
        if on:
            self.set_atr_mask (ENABLE)
            self.set_atr_txval(     0)
            self.set_atr_rxval(ENABLE)
        else:
            self.set_atr_mask (0)
            self.set_atr_txval(0)
            self.set_atr_rxval(0)

    def select_rx_antenna(self, which_antenna):
        """
        Specify which antenna port to use for reception.
        @param which_antenna: either 'TX/RX' or 'RX2'
        """
        if which_antenna in (0, 'TX/RX'):
            self._u.write_io(self._which, 0,        RX2_RX1N)
        elif which_antenna in (1, 'RX2'):
            self._u.write_io(self._which, RX2_RX1N, RX2_RX1N)
        else:
            raise ValueError, "which_antenna must be either 'TX/RX' or 'RX2'"

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        maxgain = self.gain_range()[1] - self._u.pga_max()
        mingain = self.gain_range()[0]
        if gain > maxgain:
            pga_gain = gain-maxgain
            assert pga_gain <= self._u.pga_max()
            agc_gain = maxgain
        else:
            pga_gain = 0
            agc_gain = gain
        V_maxgain = .2
        V_mingain = 1.2
        V_fullscale = 3.3
        dac_value = (agc_gain*(V_maxgain-V_mingain)/(maxgain-mingain) + V_mingain)*4096/V_fullscale
        assert dac_value>=0 and dac_value<4096
        return self._u.write_aux_dac(self._which, 0, int(dac_value)) and \
               self._set_pga(int(pga_gain))

    def set_lo_offset(self, offset):
	"""
	Set amount by which LO is offset from requested tuning frequency.
	
	@param offset: offset in Hz
	"""
	self._lo_offset = offset

    def lo_offset(self):
	"""
	Get amount by which LO is offset from requested tuning frequency.
	
	@returns Offset in Hz
	"""
	return self._lo_offset


    def i_and_q_swapped(self):
        """
        Return True if this is a quadrature device and ADC 0 is Q.
        """
        return True	

# ----------------------------------------------------------------

class _ADF410X_common(object):
    def __init__(self):
        # R-Register Common Values
        self.R_RSV = 0   # bits 23,22,21
        self.LDP = 1   # bit 20     Lock detect in 5 cycles
        self.TEST = 0  # bit 19,18  Normal
        self.ABP = 0   # bit 17,16  2.9ns

        # N-Register Common Values
        self.N_RSV = 0      # 23,22
        self.CP_GAIN = 0    # 21
        
        # Function Register Common Values
        self.P = 0        # bits 23,22    0 = 8/9, 1 = 16/17, 2 = 32/33, 3 = 64/65
        self.PD2 = 0      # bit  21       Normal operation
        self.CP2 = 7      # bits 20,19,18 CP Gain = 5mA
        self.CP1 = 7      # bits 17,16,15 CP Gain = 5mA
        self.TC = 0       # bits 14-11    PFD Timeout
        self.FL = 0       # bit 10,9      Fastlock Disabled
        self.CP3S = 0     # bit 8         CP Enabled
        self.PDP = 0      # bit 7         Phase detector polarity, Positive=1
        self.MUXOUT = 1   # bits 6:4      Digital Lock Detect
        self.PD1 = 0      # bit 3         Normal operation
        self.CR = 0       # bit 2         Normal operation

    def _compute_regs(self, freq):
        """
        Determine values of R, control, and N registers, along with actual freq.
        
        @param freq: target frequency in Hz
        @type freq: float
        @returns: (R, N, control, actual_freq)
        @rtype: tuple(int, int, int, float)
        """

        #  Band-specific N-Register Values
        phdet_freq = self._refclk_freq()/self.R_DIV
        print "phdet_freq = %f" % (phdet_freq,)
        desired_n = round(freq*self.freq_mult/phdet_freq)
        print "desired_n %f" % (desired_n,)
        actual_freq = desired_n * phdet_freq
        print "actual freq %f" % (actual_freq,)
        B = math.floor(desired_n/self._prescaler())
        A = desired_n - self._prescaler()*B
        print "A %d B %d" % (A,B)
        self.B_DIV = int(B)    # bits 20:8
        self.A_DIV = int(A)    # bit 6:2
        #assert self.B_DIV >= self.A_DIV
        if self.B_DIV < self.A_DIV:
            return (0,0,0,0)
        R = (self.R_RSV<<21) | (self.LDP<<20) | (self.TEST<<18) | \
            (self.ABP<<16) | (self.R_DIV<<2)
        
        N = (self.N_RSV<<22) | (self.CP_GAIN<<21) | (self.B_DIV<<8) | (self.A_DIV<<2)

        control = (self.P<<22) | (self.PD2<<21) | (self.CP2<<18) | (self.CP1<<15) | \
                  (self.TC<<11) | (self.FL<<9) | (self.CP3S<<8) | (self.PDP<<7) | \
                  (self.MUXOUT<<4) | (self.PD1<<3) | (self.CR<<2)

        return (R,N,control,actual_freq/self.freq_mult)

    def _write_all(self, R, N, control):
        """
        Write all PLL registers:
            R counter latch,
            N counter latch,
            Function latch,
            Initialization latch

        Adds 10ms delay between writing control and N if this is first call.
        This is the required power-up sequence.
        
        @param R: 24-bit R counter latch
        @type R: int
        @param N: 24-bit N counter latch
        @type N: int
        @param control: 24-bit control latch
        @type control: int
        """
        self._write_R(R)
        self._write_func(control)
        self._write_init(control)
        if self.first:
            time.sleep(0.010)
            self.first = False
        self._write_N(N)

    def _write_R(self, R):
        self._write_it((R & ~0x3) | 0)

    def _write_N(self, N):
        self._write_it((N & ~0x3) | 1)

    def _write_func(self, func):
        self._write_it((func & ~0x3) | 2)

    def _write_init(self, init):
        self._write_it((init & ~0x3) | 3)

    def _write_it(self, v):
        s = ''.join((chr((v >> 16) & 0xff),
                     chr((v >>  8) & 0xff),
                     chr(v & 0xff)))
        self._u._write_spi(0, self.spi_enable, self.spi_format, s)
        
    def _prescaler(self):
        if self.P == 0:
            return 8
        elif self.P == 1:
            return 16
        elif self.P == 2:
            return 32
        elif self.P == 3:
            return 64
        else:
            raise ValueError, "Prescaler out of range"

#----------------------------------------------------------------------
class _lo_common(_ADF410X_common):
    def __init__(self):
        _ADF410X_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 4  # bits 15:2
   
        # Band-specific C-Register values
        self.P = 0        # bits 23,22   0 = Div by 8/9
        self.CP2 = 7      # bits 19:17
        self.CP1 = 7      # bits 16:14

        # Band specifc N-Register Values
        self.DIVSEL = 0   # bit 23
        self.DIV2 = 0     # bit 22
        self.CPGAIN = 0   # bit 21
        self.freq_mult = 1

	self.div = 1
	self.aux_div = 2

    def freq_range(self):           # FIXME
        return (50e6, 1000e6, 16e6)

    def set_divider(self, main_or_aux, divisor):
        if main_or_aux not in (0, 'main', 1, 'aux'):
            raise ValueError, "main_or_aux must be 'main' or 'aux'"
        if main_or_aux in (0, 'main'):
            if divisor not in (1,2,4,8):
                raise ValueError, "Main Divider Must be 1, 2, 4, or 8"
            for (div,val) in ((1,0),(2,1),(4,2),(8,3)):
                if(div == divisor):
                    self.main_div = val
        else:
            if divisor not in (2,4,8,16):
                raise ValueError, "Aux Divider Must be 2, 4, 8 or 16"
            for (div,val) in ((2,0),(4,1),(8,2),(16,3)):
                if(div == divisor):
                    self.aux_div = val

        vala = self.main_div*SELA0
        valb = self.aux_div*SELB0
        mask = SELA0|SELA1|SELB0|SELB1

	self._rx_write_io(((self.main_div*SELA0) | (self.aux_div*SELB0)),
                             (SELA0|SELA1|SELB0|SELB1))

    def set_freq(self, freq):
        #freq += self._lo_offset

        if(freq < 20e6 or freq > 1200e6):
            raise ValueError, "Requested frequency out of range"
        div = 1
        lo_freq = freq * 2
        while lo_freq < 1e9 and div < 8:
            div = div * 2
            lo_freq = lo_freq * 2
        print "For RF freq of %f, we set DIV=%d and LO Freq=%f" % (freq, div, lo_freq)
        self.set_divider('main', div)
        self.set_divider('aux', div*2)

        R, N, control, actual_freq = self._compute_regs(lo_freq)
        print "R %d N %d control %d actual freq %f" % (R,N,control,actual_freq)
        if R==0:
            return(False,0)
        self._write_all(R, N, control)
        return (self._lock_detect(), actual_freq/div/2)

        
#------------------------------------------------------------    
class db_wbx_lo_tx(_lo_common, wbx_base_tx):
    def __init__(self, usrp, which):
        wbx_base_tx.__init__(self, usrp, which)
        _lo_common.__init__(self)
        
    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.

        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)

        Gain is controlled by a VGA in the output amplifier, not the PGA
        """
        return (-56, 0, 0.1)

    def set_gain(self, gain):
        """
        Set the gain.
        
        @param gain:  gain in decibels
        @returns True/False
        """
        maxgain = self.gain_range()[1]
        mingain = self.gain_range()[0]
        if gain > maxgain:
            txvga_gain = maxgain
        elif gain < mingain:
            txvga_gain = mingain
        else:
            txvga_gain = gain

        V_maxgain = 1.4
        V_mingain = 0.1
        V_fullscale = 3.3
        dac_value = ((txvga_gain-mingain)*(V_maxgain-V_mingain)/(maxgain-mingain) + V_mingain)*4096/V_fullscale
        assert dac_value>=0 and dac_value<4096
        print "DAC value %d" % (dac_value,)
        return self._u.write_aux_dac(self._which, 1, int(dac_value))

class db_wbx_lo_rx(_lo_common, wbx_base_rx):
    def __init__(self, usrp, which):
        wbx_base_rx.__init__(self, usrp, which)
        _lo_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 45, 0.05)

#------------------------------------------------------------    
# hook these daughterboard classes into the auto-instantiation framework
db_instantiator.add(usrp_dbid.WBX_LO_TX, lambda usrp, which : (db_wbx_lo_tx(usrp, which),))
db_instantiator.add(usrp_dbid.WBX_LO_RX, lambda usrp, which : (db_wbx_lo_rx(usrp, which),))


