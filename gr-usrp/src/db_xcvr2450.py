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

debug_using_gui = False                  # Must be set to True or False

#if debug_using_gui:
#    import flexrf_debug_gui

# d'board i/o pin defs

# TX IO Pins
HB_PA_OFF = (1 << 15)       # 5GHz PA, 1 = off, 0 = on
LB_PA_OFF = (1 << 14)       # 2.4GHz PA, 1 = off, 0 = on
ANTSEL_TX1_RX2 = (1 << 13)  # 1 = Ant 1 to TX, Ant 2 to RX
ANTSEL_TX2_RX1 = (1 << 12)    # 1 = Ant 2 to TX, Ant 1 to RX
TX_EN = (1 << 11)           # 1 = TX on, 0 = TX off

# RX IO Pins
LOCKDET = (1 << 15)         # This is an INPUT!!!
EN = (1 << 14)
RX_EN = (1 << 13)           # 1 = RX on, 0 = RX off
RX_HP = (1 << 12)
B1 = (1 << 11)
B2 = (1 << 10)
B3 = (1 << 9)
B4 = (1 << 8)
B5 = (1 << 7)
B6 = (1 << 6)
B7 = (1 << 5)

"""
A few comments about the XCVR2450:
  It is half-duplex.  I.e., transmit and receive are mutually exclusive.
  There is a single LO for both the Tx and Rx sides.
"""

class xcvr2450_base(db_base.db_base):
    """
    Abstract base class for all xcvr2450 boards.

    Derive board specific subclasses from db_xcvr2450_base_{tx,rx}
    """
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.source_c
        @param which: which side: 0 or 1 corresponding to side A or B respectively
        @type which: int
        """
        # sets _u  _which _tx and _slot
        db_base.db_base.__init__(self, usrp, which)

        self.spi_format = usrp1.SPI_FMT_MSB | usrp1.SPI_FMT_HDR_0
        self.spi_enable = (usrp1.SPI_ENABLE_RX_A, usrp1.SPI_ENABLE_RX_B)[which]

        # FIXME -- the write reg functions don't work with 0xffff for masks
        self._rx_write_oe(int(), 0x7fff)
        self._rx_write_io((), ())

        self._tx_write_oe((), 0x7fff)
        self._tx_write_io((), ())
        
        self.mimo = 1
        self.cp_current = 0  # 0 = 2mA, 1 = 4mA
        self.ref_div = 4  # 1 to 7

        self.rssi_hbw = 0 # 0 = 2 MHz, 1 = 6 MHz
        self.txlpf_bw = 1  # 1 = 12 MHz, 2 = 18 MHz, 3 = 24 MHz
        self.rxlpf_bw = 1 # 0 = 7.5 MHz, 1 = 9.5 MHz, 2 = 14 MHz, 3 = 18 MHz
        self.rxlpf_fine = 2 # 0 = 90%, 1 = 95%, 2 = 100%, 3 = 105%, 4 = 110%

        self.rxvga_ser = 1 # 0 = RXVGA controlled by B7:1, 1 = controlled serially
        self.txvga_ser = 1 # 0 = TXVGA controlled by B6:1, 1 = controlled serially

        self.tx_driver_lin = 2 # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_vga_lin = 2 # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_upconv_lin = 2 # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_bb_gain = 0 # 0 = maxgain-5dB, 1 = max-3dB, 2 = max-1.5dB, 3 = max

        self.pabias_delay = 15  # 0 = 0, 15 = 7uS
        self.pabias = 0  # 0 = 0 uA, 63 = 315uA
        
        self.rxgain = 64
        self.txgain = 32

        self.set_freq(2.45e6)
        self.set_auto_tr(True)
        
        #  SPI reset here
        self.set_reg_standby()
        self.set_reg_bandselpll()
        self.set_reg_cal()
        self.set_reg_lpf()
        self.set_reg_rxrssi_ctrl()
        self.set_reg_txlin_gain()
        self.set_reg_pabias()
        self.set_reg_rxgain()
        self.set_reg_txgain()
        
    def set_reg_standby(self):   # reg 2
        self.reg_standby = (self.mimo<<17)|(1<<16)|(1<<6)|(1<<5)|(1<<4)|2
        self.send_reg(self.reg_standby)
        
    def set_reg_bandselpll(self):   # reg 5
        if self.freq > 5.4e9:
            self.highband = 1
            self.five_gig = 1
        else if self.freq > 3e9:
            self.highband = 0
            self.five_gig = 1
        else:
            self.highband = 0
            self.five_gig = 0
        self.reg_bandselpll = (self.mimo<<17)|(1<<16)|(1<<15)|(1<<11)|(self.highband<<10)| \
                              (self.cp_current<<9)|(self.cp_current<<5)|(self.five_gig<<4)|5
        self.send_reg(self.reg_bandselpll)
        
    def set_reg_cal(self):   # reg 6
        # FIXME  do calibration
        self.reg_cal = (1<<14)|6
        self.send_reg(self.reg_cal)

    def set_reg_lpf(self):
        self.reg_lpf = (self.rssi_hbw<<15)|(self.txlpf_bw<<9)|(self.rxlpf_bw<<7)|(self.rxlpf_fine<<4)|7
        self.send_reg(self.reg_lpf)
        
    def set_reg_rxrssi_ctrl(self):
        self.reg_rxrssi_ctrl = (self.rxvga_serial<<16)|(1<<15)|(1<<14)|(1<<9)|(1<<4)|8
        self.send_reg(self.reg_rxrssi_ctrl)

    def set_reg_txlin_gain(self):
        self.reg_txlin_gain = (self.txvga_ser<<14)|(self.tx_driver_lin<<12)| \
                              (self.tx_vga_lin<<10)|(self.tx_upconv_lin<<6)|(self.tx_bb_gain<<4)|9
        self.send_reg(self.reg_txlin_gain)

    def set_reg_pabias(self):
        self.reg_pabias = (self.pabias_delay<<10)|(self.pabias<<4)|10
        self.send_reg(self.reg_pabias)

    def set_reg_rxgain(self):
        self.reg_rxgain = (self.rxgain<<4)|11
        self.send_reg(self.reg_rxgain)

    def set_reg_txgain(self):
        self.reg_txgain = (self.txgain<<4)|12
        self.send_reg(self.reg_txgain)
        
    def __del__(self):
        #self._u.write_io(self._which, self.power_off, POWER_UP)   # turn off power to board
        #self._u._write_oe(self._which, 0, 0xffff)   # turn off all outputs
        self.set_auto_tr(False)

    def _lock_detect(self):
        """
        @returns: the value of the VCO/PLL lock detect bit.
        @rtype: 0 or 1
        """
        if self._rx_read_io() & LOCKDET:
            return True
        else:      # Give it a second chance
            if self._rx_read_io() & LOCKDET:
                return True
            else:
                return False

    def send_reg(self,s):
        self._u._write_spi(0, self.spi_enable, self.spi_format, s)
        print "Set Reg %d: Value %x: \n" % ((s&15),s)
        
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

    def _tx_read_io(self):
        t = self._u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self._which])
        return t & 0xffff

    def _rx_read_io(self):
        t = self._u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self._which])
        return (t >> 16) & 0xffff

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

class xcvr2450_tx(xcvr2450_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.sink_c
        @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
        """
        xcvr2450_base.__init__(self, usrp, which)

        # power up the transmit side, NO -- but set antenna to receive
        self._u.write_io(self._which, (TX_POWER), (TX_POWER|RX_TXN))
        self._lo_offset = 0e6

        set_atr_mask(v)
        set_atr_txval(v)
        set_atr_rxval(v)
        set_atr_tx_delay(v)
        set_atr_rx_delay(v)

        #  Gain is not set by the PGA, but the PGA must be set at max gain in the TX
        return self._set_pga(self._u.pga_max())

    def __del__(self):
        # Power down and leave the T/R switch in the R position
        xcvr2450_base.__del__(self)

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

class xcvr2450_rx(wbx_base):
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

    def __init__(self):
        pass

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

    def gain_range(self):
        """
        Return range of gain that can be set by this d'board.
        
        @returns (min_gain, max_gain, step_size)
        Where gains are expressed in decibels (your mileage may vary)
        """
        return (self._u.pga_min(), self._u.pga_max() + 45, 0.05)

#----------------------------------------------------------------------
class _lo_common(_ADF410X_common):

    def freq_range(self):           # FIXME
        return (50e6, 1000e6, 16e6)

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
        self.set_divider('aux', 2)

        R, N, control, actual_freq = self._compute_regs(lo_freq)
        print "R %d N %d control %d actual freq %f" % (R,N,control,actual_freq)
        if R==0:
            return(False,0)
        self._write_all(R, N, control)
        return (self._lock_detect(), actual_freq/div/2)

        
#------------------------------------------------------------    
# hook these daughterboard classes into the auto-instantiation framework
db_instantiator.add(usrp_dbid.XCVR2450_TX, lambda usrp, which : (db_xcvr2450_tx(usrp, which),))
db_instantiator.add(usrp_dbid.XCVR2450_RX, lambda usrp, which : (db_xcvr2450_rx(usrp, which),))

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


