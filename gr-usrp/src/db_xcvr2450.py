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

from gnuradio import usrp1, gru, eng_notation
import time, math, weakref

from usrpm import usrp_dbid
import db_base
import db_instantiator
from usrpm.usrp_fpga_regs import *

# Convenience function
n2s = eng_notation.num_to_str

# d'board i/o pin defs

# TX IO Pins
HB_PA_OFF = (1 << 15)       # 5GHz PA, 1 = off, 0 = on
LB_PA_OFF = (1 << 14)       # 2.4GHz PA, 1 = off, 0 = on
ANTSEL_TX1_RX2 = (1 << 13)  # 1 = Ant 1 to TX, Ant 2 to RX
ANTSEL_TX2_RX1 = (1 << 12)    # 1 = Ant 2 to TX, Ant 1 to RX
TX_EN = (1 << 11)           # 1 = TX on, 0 = TX off
TX_OE_MASK = HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2|ANTSEL_TX2_RX1|TX_EN
TX_SAFE_IO = HB_PA_OFF|LB_PA_OFF|ANTSEL_TX1_RX2

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
RX_OE_MASK = EN|RX_EN|RX_HP|B1|B2|B3|B4|B5|B6|B7
RX_SAFE_IO = EN


# ------------------------------------------------------------------------
# A few comments about the XCVR2450:
#
# It is half-duplex.  I.e., transmit and receive are mutually exclusive.
# There is a single LO for both the Tx and Rx sides.
# For our purposes the board is always either receiving or transmitting.
#
# Each board is uniquely identified by the *USRP hardware* instance and side
# This dictionary holds a weak reference to existing board controller so it
# can be created or retrieved as needed.

_xcvr2450_inst = weakref.WeakValueDictionary()
def _get_or_make_xcvr2450(usrp, which):
    key = (usrp.serial_number(), which)
    if not _xcvr2450_inst.has_key(key):
	print "Creating new xcvr2450 instance"
        inst = xcvr2450(usrp, which)
        _xcvr2450_inst[key] = inst
    else:
	print "Using existing xcvr2450 instance"
        inst = _xcvr2450_inst[key]
    return inst

# ------------------------------------------------------------------------
# Common, shared object for xcvr2450 board.  Transmit and receive classes
# operate on an instance of this; one instance is created per physical
# daughterboard.

class xcvr2450(object):
    def __init__(self, usrp, which):
        print "xcvr2450: __init__ with %s: %d" % (usrp.serial_number(), which)
        self.u = usrp
        self.which = which

        # Use MSB with no header
        self.spi_format = usrp1.SPI_FMT_MSB | usrp1.SPI_FMT_HDR_0
        self.spi_enable = (usrp1.SPI_ENABLE_RX_A, usrp1.SPI_ENABLE_RX_B)[which]

        # Sane defaults
        self.mimo = 1              # 0 = OFF, 1 = ON
        self.int_div = 192         # 128 = min, 255 = max
        self.frac_div = 0          # 0 = min, 65535 = max
        self.highband = 0          # 0 = freq <= 5.4e9, 1 = freq > 5.4e9
        self.five_gig = 0          # 0 = freq <= 3.e9, 1 = freq > 3e9
        self.cp_current = 0        # 0 = 2mA, 1 = 4mA
        self.ref_div = 4           # 1 to 7
        self.rssi_hbw = 0          # 0 = 2 MHz, 1 = 6 MHz
        self.txlpf_bw = 1          # 1 = 12 MHz, 2 = 18 MHz, 3 = 24 MHz
        self.rxlpf_bw = 1          # 0 = 7.5 MHz, 1 = 9.5 MHz, 2 = 14 MHz, 3 = 18 MHz
        self.rxlpf_fine = 2        # 0 = 90%, 1 = 95%, 2 = 100%, 3 = 105%, 4 = 110%
        self.rxvga_ser = 1         # 0 = RXVGA controlled by B7:1, 1 = controlled serially
        self.rssi_range = 1        # 0 = low range (datasheet typo), 1 = high range (0.5V - 2.0V) 
        self.rssi_mode = 1         # 0 = enable follows RXHP, 1 = enabled
        self.rssi_mux = 0          # 0 = RSSI, 1 = TEMP
        self.rx_hp_pin = 0         # 0 = Fc set by rx_hpf, 1 = 600 KHz
        self.rx_hpf = 0            # 0 = 100Hz, 1 = 30KHz
        self.rx_ant = 0            # 0 = Ant. #1, 1 = Ant. #2
        self.tx_ant = 0            # 0 = Ant. #1, 1 = Ant. #2
        self.txvga_ser = 1         # 0 = TXVGA controlled by B6:1, 1 = controlled serially
        self.tx_driver_lin = 2     # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_vga_lin = 2        # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_upconv_lin = 2     # 0 = 50% (worst linearity), 1 = 63%, 2 = 78%, 3 = 100% (best lin)
        self.tx_bb_gain = 3        # 0 = maxgain-5dB, 1 = max-3dB, 2 = max-1.5dB, 3 = max
        self.pabias_delay = 15     # 0 = 0, 15 = 7uS
        self.pabias = 0            # 0 = 0 uA, 63 = 315uA
	self.rx_rf_gain = 0        # 0 = 0dB, 1 = 0dB, 2 = 15dB, 3 = 30dB
	self.rx_bb_gain = 16       # 0 = min, 31 = max (0 - 62 dB)

        self.txgain = 63           # 0 = min, 63 = max

        # Initialize GPIO and ATR
        self.tx_write_io(TX_SAFE_IO, TX_OE_MASK)
        self.tx_write_oe(TX_OE_MASK, ~0)
        self.tx_set_atr_txval(TX_SAFE_IO)
        self.tx_set_atr_rxval(TX_SAFE_IO)
        self.tx_set_atr_mask(TX_OE_MASK)
        self.rx_write_io(RX_SAFE_IO, RX_OE_MASK)
        self.rx_write_oe(RX_OE_MASK, ~0)
        self.rx_set_atr_rxval(RX_SAFE_IO)
        self.rx_set_atr_txval(RX_SAFE_IO)
        self.rx_set_atr_mask(RX_OE_MASK)
        
        # Initialize chipset
        # TODO: perform reset sequence to ensure power up defaults
        self.set_reg_standby()
        self.set_reg_bandselpll()
        self.set_reg_cal()
        self.set_reg_lpf()
        self.set_reg_rxrssi_ctrl()
        self.set_reg_txlin_gain()
        self.set_reg_pabias()
        self.set_reg_rxgain()
        self.set_reg_txgain()
        self.set_freq(2.45e9)

    def __del__(self):
        print "xcvr2450: __del__"
        self.tx_set_atr_txval(TX_SAFE_IO)
        self.tx_set_atr_rxval(TX_SAFE_IO)
        self.rx_set_atr_rxval(RX_SAFE_IO)
        self.rx_set_atr_txval(RX_SAFE_IO)
        

    # --------------------------------------------------------------------
    # These methods set the MAX2829 onboard registers over the SPI bus.
    # The SPI format is 18 bits, with the four LSBs holding the register no.
    # Thus, the shift values used here are the D0-D13 values from the data
    # sheet, *plus* four.
    
    # Standby (2)
    def set_reg_standby(self):
        self.reg_standby = (
            (self.mimo<<17) | 
            (1<<16)         | 
            (1<<6)          | 
            (1<<5)          | 
            (1<<4)          | 2)
        self.send_reg(self.reg_standby)

    # Integer-Divider Ratio (3)
    def set_reg_int_divider(self):
        self.reg_int_divider = (
            ((self.frac_div & 0x03)<<16) | 
             (self.int_div<<4)           | 3)
        self.send_reg(self.reg_int_divider)

    # Fractional-Divider Ratio (4)
    def set_reg_frac_divider(self):
        self.reg_frac_divider = ((self.frac_div & 0xfffc)<<2) | 4
        self.send_reg(self.reg_frac_divider)
        
    # Band Select and PLL (5)
    def set_reg_bandselpll(self):
        self.reg_bandselpll = (
            (self.mimo<<17)      |
            (1<<16)              |
            (1<<15)              |
            (1<<11)              |
            (self.highband<<10)  |
            (self.cp_current<<9) |
            (self.ref_div<<5)    |
            (self.five_gig<<4)   | 5)
        self.send_reg(self.reg_bandselpll)
     

    # Calibration (6)
    def set_reg_cal(self):
        # FIXME do calibration
        self.reg_cal = (1<<14)|6
        self.send_reg(self.reg_cal)


    # Lowpass Filter (7)
    def set_reg_lpf(self):
        self.reg_lpf = (
            (self.rssi_hbw<<15)  |
            (self.txlpf_bw<<10)  |
            (self.rxlpf_bw<<9)   |
            (self.rxlpf_fine<<4) | 7)
        self.send_reg(self.reg_lpf)


    # Rx Control/RSSI (8)
    def set_reg_rxrssi_ctrl(self):
        self.reg_rxrssi_ctrl = (
            (self.rxvga_ser<<16)  |
            (self.rssi_range<<15) |
            (self.rssi_mode<<14)  |
            (self.rssi_mux<<12)   |
            (1<<9)                |
            (self.rx_hpf<<6)      |
            (1<<4) 		  | 8)
        self.send_reg(self.reg_rxrssi_ctrl)


    # Tx Linearity/Baseband Gain (9)
    def set_reg_txlin_gain(self):
        self.reg_txlin_gain = (
            (self.txvga_ser<<14)     |
            (self.tx_driver_lin<<12) |
            (self.tx_vga_lin<<10)    |
            (self.tx_upconv_lin<<6)  |
            (self.tx_bb_gain<<4)     | 9)
        self.send_reg(self.reg_txlin_gain)


    # PA Bias DAC (10)
    def set_reg_pabias(self):
        self.reg_pabias = (
            (self.pabias_delay<<10) |
            (self.pabias<<4)        | 10)
        self.send_reg(self.reg_pabias)


    # Rx Gain (11)
    def set_reg_rxgain(self):
        self.reg_rxgain = (
	    (self.rx_rf_gain<<9) |
	    (self.rx_bb_gain<<4) | 11)
        self.send_reg(self.reg_rxgain)


    # Tx Gain (12)
    def set_reg_txgain(self):
        self.reg_txgain = (self.txgain<<4) | 12
        self.send_reg(self.reg_txgain)


    # Send register write to SPI
    def send_reg(self, v):
        # Send 24 bits, it keeps last 18 clocked in
        s = ''.join((chr((v >> 16) & 0xff),
                     chr((v >>  8) & 0xff),
                     chr(v & 0xff)))
        self.u._write_spi(0, self.spi_enable, self.spi_format, s)
        print "xcvr2450: Setting reg %d to %06X" % ((v&15), v)

    # --------------------------------------------------------------------
    # These methods control the GPIO bus.  Since the board has to access
    # both the io_rx_* and io_tx_* pins, we define our own methods to do so.
    # This bypasses any code in db_base.
    #
    # The board operates in ATR mode, always.  Thus, when the board is first
    # initialized, it is in receive mode, until bits show up in the TX FIFO.
    #
    def tx_write_oe(self, value, mask):
        return self.u._write_fpga_reg((FR_OE_0, FR_OE_2)[self.which],
                                       gru.hexint((mask << 16) | value))
    
    def tx_write_io(self, value, mask):
        return self.u._write_fpga_reg((FR_IO_0, FR_IO_2)[self.which],
                                       gru.hexint((mask << 16) | value))

    def tx_read_io(self):
        t = self.u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self.which])
        return t & 0xffff


    def rx_write_oe(self, value, mask):
        return self.u._write_fpga_reg((FR_OE_1, FR_OE_3)[self.which],
                                       gru.hexint((mask << 16) | value))

    def rx_write_io(self, value, mask):
        return self.u._write_fpga_reg((FR_IO_1, FR_IO_3)[self.which],
                                       gru.hexint((mask << 16) | value))

    def rx_read_io(self):
        t = self.u._read_fpga_reg((FR_RB_IO_RX_A_IO_TX_A, FR_RB_IO_RX_B_IO_TX_B)[self.which])
        return (t >> 16) & 0xffff

    def tx_set_atr_mask(self, v):
        return self.u._write_fpga_reg((FR_ATR_MASK_0,FR_ATR_MASK_2)[self.which],
                                       gru.hexint(v))

    def tx_set_atr_txval(self, v):
        return self.u._write_fpga_reg((FR_ATR_TXVAL_0,FR_ATR_TXVAL_2)[self.which],
                                       gru.hexint(v))

    def tx_set_atr_rxval(self, v):
        return self.u._write_fpga_reg((FR_ATR_RXVAL_0,FR_ATR_RXVAL_2)[self.which],
                                       gru.hexint(v))

    def rx_set_atr_mask(self, v):
        return self.u._write_fpga_reg((FR_ATR_MASK_1,FR_ATR_MASK_3)[self.which],
                                       gru.hexint(v))

    def rx_set_atr_txval(self, v):
        return self.u._write_fpga_reg((FR_ATR_TXVAL_1,FR_ATR_TXVAL_3)[self.which],
                                       gru.hexint(v))

    def rx_set_atr_rxval(self, v):
        return self.u._write_fpga_reg((FR_ATR_RXVAL_1,FR_ATR_RXVAL_3)[self.which],
                                       gru.hexint(v))

    def set_gpio(self):
        # We calculate four values:
        #
        # io_rx_while_rx: what to drive onto io_rx_* when receiving
        # io_rx_while_tx: what to drive onto io_rx_* when transmitting
        # io_tx_while_rx: what to drive onto io_tx_* when receiving
        # io_tx_while_tx: what to drive onto io_tx_* when transmitting
        #
        # B1-B7 is ignored as gain is set serially for now.

        rx_hp     = (0, RX_HP)[self.rx_hp_pin]
        tx_antsel = (ANTSEL_TX1_RX2, ANTSEL_TX2_RX1)[self.tx_ant] 
        rx_antsel = (ANTSEL_TX1_RX2, ANTSEL_TX2_RX1)[self.rx_ant] 
        tx_pa_sel = (HB_PA_OFF, LB_PA_OFF)[self.five_gig]
        io_rx_while_rx = EN|rx_hp|RX_EN
        io_rx_while_tx = EN|rx_hp
        io_tx_while_rx = HB_PA_OFF|LB_PA_OFF|rx_antsel
        io_tx_while_tx = tx_pa_sel|tx_antsel|TX_EN
        self.rx_set_atr_rxval(io_rx_while_rx)
        self.rx_set_atr_txval(io_rx_while_tx)
        self.tx_set_atr_rxval(io_tx_while_rx)
        self.tx_set_atr_txval(io_tx_while_tx)
        
        print "GPIO: RXRX=%04X RXTX=%04X TXRX=%04X TXTX=%04X" % (
            io_rx_while_rx, io_rx_while_tx, io_tx_while_rx, io_tx_while_tx)

    # --------------------------------------------------------------------
    # These methods set control the high-level operating parameters.

    def set_freq(self, target_freq):
        if target_freq > 3e9:
            self.five_gig = 1
            self.ref_div = 3
            scaler = 4.0/5.0
        else:
            self.five_gig = 0
            self.ref_div = 4
            scaler = 4.0/3.0;

        if target_freq > 5.4e9:
            self.highband = 1
        else:
            self.highband = 0

        vco_freq = target_freq*scaler;
        ref_clk = self.u.fpga_master_clock_freq()  # Assumes AD9515 is bypassed
        phdet_freq = ref_clk/self.ref_div
        div = vco_freq/phdet_freq
        self.int_div = int(math.floor(div))
        self.frac_div = int((div-self.int_div)*65536.0)
        actual_freq = phdet_freq*(self.int_div+(self.frac_div/65536.0))/scaler

        print "RF=%s VCO=%s R=%d PHD=%s DIV=%3.5f I=%3d F=%5d ACT=%s" % (
            n2s(target_freq), n2s(vco_freq), self.ref_div, n2s(phdet_freq),
            div, self.int_div, self.frac_div, n2s(actual_freq))

        self.set_gpio()
        self.set_reg_int_divider()
        self.set_reg_frac_divider()
        self.set_reg_bandselpll()

        ok = self.lock_detect()
        print "lock detect:", ok
        return (ok, actual_freq)

    def lock_detect(self):
        """
        @returns: the value of the VCO/PLL lock detect bit.
        @rtype: 0 or 1
        """
        if self.rx_read_io() & LOCKDET:
            return True
        else:      # Give it a second chance
            if self.rx_read_io() & LOCKDET:
                return True
            else:
                return False

    def set_rx_gain(self, gain):
	if gain < 0.0: gain = 0.0
	if gain > 92.0: gain = 92.0
	
	# Split the gain between RF and baseband
	# This is experimental, not prescribed
	if gain < 31.0:
	    self.rx_rf_gain = 0		           # 0 dB RF gain
	    self.rx_bb_gain = int(gain/2.0)
	    
	if gain >= 30.0 and gain < 60.5:
	    self.rx_rf_gain = 2                    # 15 dB RF gain
	    self.rx_bb_gain = int((gain-15.0)/2.0)
    
	if gain >= 60.5:
	    self.rx_rf_gain = 3			   # 30.5 dB RF gain
	    self.rx_bb_gain = int((gain-30.5)/2.0)
    
        self.set_reg_rxgain()

    def set_tx_gain(self, gain):
	if gain < 0.0: gain = 0.0
	if gain > 30.0: gain = 30.0
        self.txgain = int((gain/30.0)*63)
        self.set_reg_txgain()

class db_xcvr2450_base(db_base.db_base):
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
        self.xcvr = _get_or_make_xcvr2450(usrp, which)
        
    def set_freq(self, target_freq):
        """
        @returns (ok, actual_baseband_freq) where:
           ok is True or False and indicates success or failure,
           actual_baseband_freq is the RF frequency that corresponds to DC in the IF.
        """
        return self.xcvr.set_freq(target_freq)

    def is_quadrature(self):
        """
        Return True if this board requires both I & Q analog channels.

        This bit of info is useful when setting up the USRP Rx mux register.
        """
        return True

    def freq_range(self):
	return (2.4e9, 6e9, 1e6)

    def set_freq(self, target_freq):
        return self.xcvr.set_freq(target_freq)

# ----------------------------------------------------------------

class db_xcvr2450_tx(db_xcvr2450_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.sink_c
        @param which: 0 or 1 corresponding to side TX_A or TX_B respectively.
        """
	print "db_xcvr2450_tx: __init__"
        db_xcvr2450_base.__init__(self, usrp, which)

    def gain_range(self):
	return (0, 30, (30.0/63.0))

    def set_gain(self, gain):
        return self.xcvr.set_tx_gain(gain)
        

class db_xcvr2450_rx(db_xcvr2450_base):
    def __init__(self, usrp, which):
        """
        @param usrp: instance of usrp.source_c
        @param which: 0 or 1 corresponding to side RX_A or RX_B respectively.
        """
	print "db_xcvr2450_rx: __init__"
        db_xcvr2450_base.__init__(self, usrp, which)

    def gain_range(self):
	return (0.0, 92.0, 1)

    def set_gain(self, gain):
        return self.xcvr.set_rx_gain(gain)
        
#------------------------------------------------------------    
# hook these daughterboard classes into the auto-instantiation framework
db_instantiator.add(usrp_dbid.XCVR2450_TX, lambda usrp, which : (db_xcvr2450_tx(usrp, which),))
db_instantiator.add(usrp_dbid.XCVR2450_RX, lambda usrp, which : (db_xcvr2450_rx(usrp, which),))
