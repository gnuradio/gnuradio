#
# Copyright 2005,2007 Free Software Foundation, Inc.
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

from gnuradio import usrp1
import time,math

from usrpm import usrp_dbid
import db_base
import db_instantiator
from usrpm.usrp_fpga_regs import *

#debug_using_gui = True                  # Must be set to True or False
debug_using_gui = False                  # Must be set to True or False

if debug_using_gui:
    import flexrf_debug_gui

# d'board i/o pin defs
# Tx and Rx have shared defs, but different i/o regs
AUX_RXAGC = (1 << 8)
POWER_UP = (1 << 7)         # enables power supply
RX_TXN = (1 << 6)           # Tx only: T/R antenna switch for TX/RX port
RX2_RX1N = (1 << 6)         # Rx only: antenna switch between RX2 and TX/RX port
ENABLE = (1 << 5)           # enables mixer
AUX_SEN = (1 << 4)
AUX_SCLK = (1 << 3)
PLL_LOCK_DETECT = (1 << 2)
AUX_SDO = (1 << 1)
CLOCK_OUT = (1 << 0)

SPI_ENABLE_TX_A = usrp1.SPI_ENABLE_TX_A
SPI_ENABLE_TX_B = usrp1.SPI_ENABLE_TX_B
SPI_ENABLE_RX_A = usrp1.SPI_ENABLE_RX_A
SPI_ENABLE_RX_B = usrp1.SPI_ENABLE_RX_B

class flexrf_base(db_base.db_base):
    """
    Abstract base class for all flexrf boards.

    Derive board specific subclasses from db_flexrf_base_{tx,rx}
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

        self._u._write_oe(self._which, 0, 0xffff)   # turn off all outputs
        self._enable_refclk(False)                      # disable refclk

        g = self.gain_range()                       # initialize gain
        self.set_gain(float(g[0]+g[1]) / 2)

        self.set_auto_tr(False)
        
        if debug_using_gui:
            title = "FlexRF Debug Rx"
            if self._tx:
                title = "FlexRF Debug Tx"
            self.gui = flexrf_debug_gui.flexrf_debug_gui(self, title)
            self.gui.Show(True)


    def __del__(self):
        #print "flexrf_base.__del__"
        self._u.write_io(self._which, self.power_off, POWER_UP)   # turn off power to board
	# Power down VCO/PLL
	self.PD = 3 
	self._write_control(self._compute_control_reg())
        self._enable_refclk(False)                       # turn off refclk
        self.set_auto_tr(False)

    def _write_all(self, R, control, N):
        """
        Write R counter latch, control latch and N counter latch to VCO.

        Adds 10ms delay between writing control and N if this is first call.
        This is the required power-up sequence.
        
        @param R: 24-bit R counter latch
        @type R: int
        @param control: 24-bit control latch
        @type control: int
        @param N: 24-bit N counter latch
        @type N: int
        """
        self._write_R(R)
        self._write_control( control)
        if self.first:
            time.sleep(0.010)
            self.first = False
        self._write_N(N)

    def _write_control(self, control):
        self._write_it((control & ~0x3) | 0)

    def _write_R(self, R):
        self._write_it((R & ~0x3) | 1)

    def _write_N(self, N):
        self._write_it((N & ~0x3) | 2)

    def _write_it(self, v):
        s = ''.join((chr((v >> 16) & 0xff),
                     chr((v >>  8) & 0xff),
                     chr(v & 0xff)))
        self._u._write_spi(0, self.spi_enable, self.spi_format, s)
        
    def _lock_detect(self):
        """
        @returns: the value of the VCO/PLL lock detect bit.
        @rtype: 0 or 1
        """
        if self._u.read_io(self._which) & PLL_LOCK_DETECT:
            return True
        else:      # Give it a second chance
            if self._u.read_io(self._which) & PLL_LOCK_DETECT:
                return True
            else:
                return False
        
    def _compute_regs(self, freq):
        """
        Determine values of R, control, and N registers, along with actual freq.
        
        @param freq: target frequency in Hz
        @type freq: float
        @returns: (R, control, N, actual_freq)
        @rtype: tuple(int, int, int, float)
        
        Override this in derived classes.
        """
        raise NotImplementedError

    def _refclk_freq(self):
        # return float(self._u.fpga_master_clock_freq())/self._refclk_divisor()
        return 64e6/self._refclk_divisor()

    def set_freq(self, freq):
        """
        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """

        # Offsetting the LO helps get the Tx carrier leakage out of the way.
        # This also ensures that on Rx, we're not getting hosed by the
        # FPGA's DC removal loop's time constant.  We were seeing a
        # problem when running with discontinuous transmission.
        # Offsetting the LO made the problem go away.
        freq += self._lo_offset
        
        R, control, N, actual_freq = self._compute_regs(freq)
        if R==0:
            return(False,0)
        self._write_all(R, control, N)
        return (self._lock_detect(), actual_freq)

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
        return self._set_pga(gain)

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
	
# ----------------------------------------------------------------

class flexrf_base_tx(flexrf_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.sink_c
        @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
        """
        flexrf_base.__init__(self, usrp, which)
        self.spi_enable = (SPI_ENABLE_TX_A, SPI_ENABLE_TX_B)[which]

        # power up the transmit side, but don't enable the mixer
        self._u._write_oe(self._which,(POWER_UP|RX_TXN|ENABLE), 0xffff)
        self._u.write_io(self._which, (self.power_on|RX_TXN), (POWER_UP|RX_TXN|ENABLE))
        self.set_lo_offset(4e6)

    def __del__(self):
        #print "flexrf_base_tx.__del__"
        # Power down and leave the T/R switch in the R position
        self._u.write_io(self._which, (self.power_off|RX_TXN), (POWER_UP|RX_TXN|ENABLE))
        flexrf_base.__del__(self)

    def set_auto_tr(self, on):
        if on:
            self.set_atr_mask (RX_TXN | ENABLE)
            self.set_atr_txval(0      | ENABLE)
            self.set_atr_rxval(RX_TXN | 0)
        else:
            self.set_atr_mask (0)
            self.set_atr_txval(0)
            self.set_atr_rxval(0)

    def set_enable(self, on):
        """
        Enable transmitter if on is True
        """
        mask = RX_TXN | ENABLE
        if on:
            v = ENABLE
        else:
            v = RX_TXN
        self._u.write_io(self._which, v, mask)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.

        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)

        Flex Tx boards require that the PGA be maxed out to properly bias their circuitry.
        """
        g = self._u.pga_max()
        return (g, g, 1.0)

    def set_gain(self, gain):
        """
        Set the gain.

        @param gain:  gain in decibels
        @returns True/False
        """
        return self._set_pga(self._u.pga_max())

class flexrf_base_rx(flexrf_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.source_c
        @param which: 0 or 1 corresponding to side RX_A or RX_B respectively.
        """
        flexrf_base.__init__(self, usrp, which)
        self.spi_enable = (SPI_ENABLE_RX_A, SPI_ENABLE_RX_B)[which]
        
        self._u._write_oe(self._which, (POWER_UP|RX2_RX1N|ENABLE), 0xffff)
        self._u.write_io(self._which,  (self.power_on|RX2_RX1N|ENABLE), (POWER_UP|RX2_RX1N|ENABLE))

        # set up for RX on TX/RX port
        self.select_rx_antenna('TX/RX')

        self.bypass_adc_buffers(True)
        self.set_lo_offset(-4e6)

    def __del__(self):
        # print "flexrf_base_rx.__del__"
        # Power down
        self._u.write_io(self._which, self.power_off, (POWER_UP|ENABLE))
        flexrf_base.__del__(self)
    
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

# ----------------------------------------------------------------

class _AD4360_common(object):
    def __init__(self):
        # R-Register Common Values
        self.R_RSV = 0   # bits 23,22
        self.BSC = 3   # bits 21,20 Div by 8 to be safe
        self.TEST = 0  # bit 19
        self.LDP = 1   # bit 18
        self.ABP = 0   # bit 17,16   3ns

        # N-Register Common Values
        self.N_RSV = 0      # bit 7
        
        # Control Register Common Values
        self.PD = 0       # bits 21,20   Normal operation
        self.PL = 0       # bits 13,12   11mA
        self.MTLD = 1     # bit 11       enabled
        self.CPG = 0      # bit 10       CP setting 1
        self.CP3S = 0     # bit 9        Normal
        self.PDP = 1      # bit 8        Positive
        self.MUXOUT = 1   # bits 7:5     Digital Lock Detect
        self.CR = 0       # bit 4        Normal
        self.PC = 1       # bits 3,2     Core power 10mA

    def _compute_regs(self, freq):
        """
        Determine values of R, control, and N registers, along with actual freq.
        
        @param freq: target frequency in Hz
        @type freq: float
        @returns: (R, control, N, actual_freq)
        @rtype: tuple(int, int, int, float)
        """

        #  Band-specific N-Register Values
        phdet_freq = self._refclk_freq()/self.R_DIV
        desired_n = round(freq*self.freq_mult/phdet_freq)
        actual_freq = desired_n * phdet_freq
        B = math.floor(desired_n/self._prescaler())
        A = desired_n - self._prescaler()*B
        self.B_DIV = int(B)    # bits 20:8
        self.A_DIV = int(A)    # bit 6:2
        #assert self.B_DIV >= self.A_DIV
        if self.B_DIV < self.A_DIV:
            return (0,0,0,0)
        R = (self.R_RSV<<22) | (self.BSC<<20) | (self.TEST<<19) | (self.LDP<<18) \
            | (self.ABP<<16) | (self.R_DIV<<2)
        
        control = self._compute_control_reg()

        N = (self.DIVSEL<<23) | (self.DIV2<<22) | (self.CPGAIN<<21) | (self.B_DIV<<8) | \
            (self.N_RSV<<7) | (self.A_DIV<<2)

        return (R,control,N,actual_freq/self.freq_mult)

    def _compute_control_reg(self):
        control = (self.P<<22) | (self.PD<<20) | (self.CP2<<17) | (self.CP1<<14) | (self.PL<<12) \
                  | (self.MTLD<<11) | (self.CPG<<10) | (self.CP3S<<9) | (self.PDP<<8) | \
                  (self.MUXOUT<<5) | (self.CR<<4) | (self.PC<<2)
	return control    

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
    def _prescaler(self):
        if self.P == 0:
            return 8
        elif self.P == 1:
            return 16
        else:
            return 32

#----------------------------------------------------------------------
class _2400_common(_AD4360_common):
    def __init__(self):
        _AD4360_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 16  # bits 15:2
   
        # Band-specific C-Register values
        self.P = 1        # bits 23,22   Div by 16/17
        self.CP2 = 7      # bits 19:17
        self.CP1 = 7      # bits 16:14

        # Band specifc N-Register Values
        self.DIVSEL = 0   # bit 23
        self.DIV2 = 0     # bit 22
        self.CPGAIN = 0   # bit 21
        self.freq_mult = 1

    def freq_range(self):           # FIXME
        return (2300e6, 2700e6, 4e6)

#----------------------------------------------------------------------
class _1200_common(_AD4360_common):
    def __init__(self):
        _AD4360_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 16  # bits 15:2  DIV by 16 for a 1 MHz phase detector freq
   
        # Band-specific C-Register values
        self.P = 1        # bits 23,22   Div by 16/17
        self.CP2 = 7      # bits 19:17   1.25 mA
        self.CP1 = 7      # bits 16:14   1.25 mA

        # Band specifc N-Register Values
        self.DIVSEL = 0   # bit 23
        self.DIV2 = 1     # bit 22
        self.CPGAIN = 0   # bit 21
        self.freq_mult = 2

    def freq_range(self):           # FIXME
        return (1150e6, 1350e6, 4e6)

#-------------------------------------------------------------------------
class _1800_common(_AD4360_common):
    def __init__(self):
        _AD4360_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 16  # bits 15:2  DIV by 16 for a 1 MHz phase detector freq
   
        # Band-specific C-Register values
        self.P = 1        # bits 23,22   Div by 16/17
        self.CP2 = 7      # bits 19:17   1.25 mA
        self.CP1 = 7      # bits 16:14   1.25 mA

        # Band specifc N-Register Values
        self.DIVSEL = 0   # bit 23
        self.DIV2 = 0     # bit 22
        self.freq_mult = 1
        self.CPGAIN = 0   # bit 21

    def freq_range(self):           # FIXME
        return (1600e6, 2000e6, 4e6)

#-------------------------------------------------------------------------
class _900_common(_AD4360_common):
    def __init__(self):
        _AD4360_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 16  # bits 15:2  DIV by 16 for a 1 MHz phase detector freq
   
        # Band-specific C-Register values
        self.P = 1        # bits 23,22   Div by 16/17
        self.CP2 = 7      # bits 19:17   1.25 mA
        self.CP1 = 7      # bits 16:14   1.25 mA

        # Band specifc N-Register Values
        self.DIVSEL = 0   # bit 23
        self.DIV2 = 1     # bit 22
        self.freq_mult = 2
        self.CPGAIN = 0   # bit 21

    def freq_range(self):           # FIXME
        return (800e6, 1000e6, 4e6)

#-------------------------------------------------------------------------
class _400_common(_AD4360_common):
    def __init__(self):
        _AD4360_common.__init__(self)

        # Band-specific R-Register Values
        self.R_DIV = 16  # bits 15:2 
   
        # Band-specific C-Register values
        self.P = 0        # bits 23,22   Div by 8/9
        self.CP2 = 7      # bits 19:17   1.25 mA
        self.CP1 = 7      # bits 16:14   1.25 mA

        # Band specifc N-Register Values  These are different for TX/RX
        self.DIVSEL = 0   # bit 23
        if self._tx:
            self.DIV2 = 1 # bit 22
        else:
            self.DIV2 = 0 # bit 22   # RX side has built-in DIV2 in AD8348
        self.freq_mult = 2

        self.CPGAIN = 0   # bit 21

    def freq_range(self):           
        #return (350e6, 465e6, 1e6)    # FIXME prototype
        return (400e6, 500e6, 1e6)     # final version
    

#------------------------------------------------------------    
class db_flexrf_2400_tx(_2400_common, flexrf_base_tx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_tx.__init__(self, usrp, which)
        _2400_common.__init__(self)
        
class db_flexrf_2400_rx(_2400_common, flexrf_base_rx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0   # Powering it off kills the serial bus
        flexrf_base_rx.__init__(self, usrp, which)
        _2400_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 70, 0.05)

    def i_and_q_swapped(self):
        return True

class db_flexrf_1200_tx(_1200_common, flexrf_base_tx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_tx.__init__(self, usrp, which)
        _1200_common.__init__(self)
        
class db_flexrf_1200_rx(_1200_common, flexrf_base_rx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_rx.__init__(self, usrp, which)
        _1200_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 70, 0.05)

    def i_and_q_swapped(self):
        return True

class db_flexrf_1800_tx(_1800_common, flexrf_base_tx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_tx.__init__(self, usrp, which)
        _1800_common.__init__(self)
        
class db_flexrf_1800_rx(_1800_common, flexrf_base_rx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_rx.__init__(self, usrp, which)
        _1800_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 70, 0.05)

    def i_and_q_swapped(self):
        return True

class db_flexrf_900_tx(_900_common, flexrf_base_tx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_tx.__init__(self, usrp, which)
        _900_common.__init__(self)
        
class db_flexrf_900_rx(_900_common, flexrf_base_rx):
    def __init__(self, usrp, which):
        self.power_on = 0
        self.power_off = 0    # powering it off kills the serial bus
        flexrf_base_rx.__init__(self, usrp, which)
        _900_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 70, 0.05)

    def i_and_q_swapped(self):
        return True

class db_flexrf_400_tx(_400_common, flexrf_base_tx):
    def __init__(self, usrp, which):
        self.power_on = POWER_UP
        self.power_off = 0
        flexrf_base_tx.__init__(self, usrp, which)
        _400_common.__init__(self)
        
class db_flexrf_400_rx(_400_common, flexrf_base_rx):
    def __init__(self, usrp, which):
        self.power_on = POWER_UP
        self.power_off = 0
        flexrf_base_rx.__init__(self, usrp, which)
        _400_common.__init__(self)

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 45, 0.035)

    def i_and_q_swapped(self):
        return True
    
# hook these daughterboard classes into the auto-instantiation framework

db_instantiator.add(usrp_dbid.FLEX_2400_TX, lambda usrp, which : (db_flexrf_2400_tx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_2400_RX, lambda usrp, which : (db_flexrf_2400_rx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_TX, lambda usrp, which : (db_flexrf_1200_tx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_RX, lambda usrp, which : (db_flexrf_1200_rx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_TX, lambda usrp, which : (db_flexrf_1800_tx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_RX, lambda usrp, which : (db_flexrf_1800_rx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_TX,  lambda usrp, which : (db_flexrf_900_tx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_RX,  lambda usrp, which : (db_flexrf_900_rx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_TX,  lambda usrp, which : (db_flexrf_400_tx(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_RX,  lambda usrp, which : (db_flexrf_400_rx(usrp, which),))
