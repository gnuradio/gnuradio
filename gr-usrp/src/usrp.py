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



from usrpm import usrp_prims
from usrpm import usrp_dbid
from gnuradio import usrp1              # usrp Rev 1 and later
from gnuradio import gru
from usrpm.usrp_fpga_regs import *

FPGA_MODE_NORMAL   = usrp1.FPGA_MODE_NORMAL
FPGA_MODE_LOOPBACK = usrp1.FPGA_MODE_LOOPBACK
FPGA_MODE_COUNTING = usrp1.FPGA_MODE_COUNTING

SPI_FMT_xSB_MASK = usrp1.SPI_FMT_xSB_MASK
SPI_FMT_LSB      = usrp1.SPI_FMT_LSB
SPI_FMT_MSB      = usrp1.SPI_FMT_MSB
SPI_FMT_HDR_MASK = usrp1.SPI_FMT_HDR_MASK
SPI_FMT_HDR_0    = usrp1.SPI_FMT_HDR_0
SPI_FMT_HDR_1    = usrp1.SPI_FMT_HDR_1
SPI_FMT_HDR_2    = usrp1.SPI_FMT_HDR_2

SPI_ENABLE_FPGA     = usrp1.SPI_ENABLE_FPGA
SPI_ENABLE_CODEC_A  = usrp1.SPI_ENABLE_CODEC_A
SPI_ENABLE_CODEC_B  = usrp1.SPI_ENABLE_CODEC_B
SPI_ENABLE_reserved = usrp1.SPI_ENABLE_reserved
SPI_ENABLE_TX_A     = usrp1.SPI_ENABLE_TX_A
SPI_ENABLE_RX_A     = usrp1.SPI_ENABLE_RX_A
SPI_ENABLE_TX_B     = usrp1.SPI_ENABLE_TX_B
SPI_ENABLE_RX_B     = usrp1.SPI_ENABLE_RX_B


# Import all the daughterboard classes we know about.
# This hooks them into the auto-instantiation framework.

import db_instantiator

import db_basic
import db_dbs_rx
import db_flexrf
import db_flexrf_mimo
import db_tv_rx


def _look_for_usrp(which):
    """
    Try to open the specified usrp.

    @param which: int >= 0 specifying which USRP to open
    @type which: int
    
    @return: Returns version number, or raises RuntimeError
    @rtype: int
    """
    d = usrp_prims.usrp_find_device(which)
    if not d:
        raise RuntimeError, "Unable to find USRP #%d" % (which,)

    return usrp_prims.usrp_hw_rev(d)


def _ensure_rev2(which):
    v = _look_for_usrp(which)
    if not v in (2, 4):
        raise RuntimeError, "Sorry, unsupported USRP revision (rev=%d)" % (v,)


class tune_result(object):
    """
    Container for intermediate tuning information.
    """
    def __init__(self, baseband_freq, dxc_freq, residual_freq, inverted):
        self.baseband_freq = baseband_freq
        self.dxc_freq = dxc_freq
        self.residual_freq = residual_freq
        self.inverted = inverted


def tune(u, chan, subdev, target_freq):
    """
    Set the center frequency we're interested in.

    @param u: instance of usrp.source_* or usrp.sink_*
    @param chan: DDC/DUC channel
    @type  chan: int
    @param subdev: daughterboard subdevice
    @param target_freq: frequency in Hz
    @returns False if failure else tune_result
    
    Tuning is a two step process.  First we ask the front-end to
    tune as close to the desired frequency as it can.  Then we use
    the result of that operation and our target_frequency to
    determine the value for the digital down converter.
    """

    # Does this usrp instance do Tx or Rx?
    rx_p = True
    try:
        u.rx_freq
    except AttributeError:
        rx_p = False

    ok, baseband_freq = subdev.set_freq(target_freq)
    dxc_freq, inverted = calc_dxc_freq(target_freq, baseband_freq, u.converter_rate())

    # If the spectrum is inverted, and the daughterboard doesn't do
    # quadrature downconversion, we can fix the inversion by flipping the
    # sign of the dxc_freq...  (This only happens using the basic_rx board)

    if subdev.spectrum_inverted():
        inverted = not(inverted)
    
    if inverted and not(subdev.is_quadrature()):
        dxc_freq = -dxc_freq
        inverted = not(inverted)

    if rx_p:
        ok = ok and u.set_rx_freq(chan, dxc_freq)
    else:
        dxc_freq = -dxc_freq
        ok = ok and u.set_tx_freq(chan, dxc_freq)
        
    if not(ok):
        return False

    # residual_freq is the offset left over because of dxc tuning step size
    if rx_p:
        residual_freq = dxc_freq - u.rx_freq(chan)
    else:
        # FIXME 50-50 chance this has the wrong sign...
        residual_freq = dxc_freq - u.tx_freq(chan)

    return tune_result(baseband_freq, dxc_freq, residual_freq, inverted)
    
    
# ------------------------------------------------------------------------
# Build subclasses of raw usrp1.* class that add the db attribute
# by automatically instantiating the appropriate daughterboard classes.
# [Also provides keyword args.]
# ------------------------------------------------------------------------

class usrp_common(object):
    def __init__(self):
        # read capability register
        r = self._u._read_fpga_reg(FR_RB_CAPS)
        if r < 0:
            r += 2**32
        if r == 0xaa55ff77:    # value of this reg prior to being defined as cap reg
            r = ((2 << bmFR_RB_CAPS_NDUC_SHIFT)
                 | (2 << bmFR_RB_CAPS_NDDC_SHIFT)
                 | bmFR_RB_CAPS_RX_HAS_HALFBAND)
        self._fpga_caps = r

        if False:
            print "FR_RB_CAPS      = %#08x" % (self._fpga_caps,)
            print "has_rx_halfband =", self.has_rx_halfband()
            print "nDDCs           =", self.nddc()
            print "has_tx_halfband =", self.has_tx_halfband()
            print "nDUCs           =", self.nduc()
    
    def __getattr__(self, name):
        return getattr(self._u, name)

    def tune(self, chan, subdev, target_freq):
        return tune(self, chan, subdev, target_freq)

    def has_rx_halfband(self):
        return self._fpga_caps & bmFR_RB_CAPS_RX_HAS_HALFBAND != 0

    def has_tx_halfband(self):
        return self._fpga_caps & bmFR_RB_CAPS_TX_HAS_HALFBAND != 0

    def nddc(self):
        """
        Number of Digital Down Converters implemented in FPGA
        """
        return (self._fpga_caps & bmFR_RB_CAPS_NDDC_MASK) >> bmFR_RB_CAPS_NDDC_SHIFT

    def nduc(self):
        """
        Number of Digital Up Converters implemented in FPGA
        """
        return (self._fpga_caps & bmFR_RB_CAPS_NDUC_MASK) >> bmFR_RB_CAPS_NDUC_SHIFT


class sink_c(usrp_common):
    def __init__(self, which=0, interp_rate=128, nchan=1, mux=0x98,
                 fusb_block_size=0, fusb_nblocks=0,
                 fpga_filename="", firmware_filename=""):
        _ensure_rev2(which)
        self._u = usrp1.sink_c(which, interp_rate, nchan, mux,
                               fusb_block_size, fusb_nblocks,
                               fpga_filename, firmware_filename)
        # Add the db attribute, which contains a 2-tuple of tuples of daughterboard classes
        self.db = (db_instantiator.instantiate(self._u, 0),
                   db_instantiator.instantiate(self._u, 1))
        usrp_common.__init__(self)

    def __del__(self):
        self.db = None          # will fire d'board destructors
        self._u = None          # will fire usrp1.* destructor


class sink_s(usrp_common):
    def __init__(self, which=0, interp_rate=128, nchan=1, mux=0x98,
                 fusb_block_size=0, fusb_nblocks=0,
                 fpga_filename="", firmware_filename=""):
        _ensure_rev2(which)
        self._u = usrp1.sink_s(which, interp_rate, nchan, mux,
                               fusb_block_size, fusb_nblocks,
                               fpga_filename, firmware_filename)
        # Add the db attribute, which contains a 2-tuple of tuples of daughterboard classes
        self.db = (db_instantiator.instantiate(self._u, 0),
                   db_instantiator.instantiate(self._u, 1))
        usrp_common.__init__(self)

    def __del__(self):
        self.db = None          # will fire d'board destructors
        self._u = None          # will fire usrp1.* destructor
        

class source_c(usrp_common):
    def __init__(self, which=0, decim_rate=64, nchan=1, mux=0x32103210, mode=0,
                 fusb_block_size=0, fusb_nblocks=0,
                 fpga_filename="", firmware_filename=""):
        _ensure_rev2(which)
        self._u = usrp1.source_c(which, decim_rate, nchan, mux, mode,
                                 fusb_block_size, fusb_nblocks,
                                 fpga_filename, firmware_filename)
        # Add the db attribute, which contains a 2-tuple of tuples of daughterboard classes
        self.db = (db_instantiator.instantiate(self._u, 0),
                   db_instantiator.instantiate(self._u, 1))
        usrp_common.__init__(self)

    def __del__(self):
        self.db = None          # will fire d'board destructors
        self._u = None          # will fire usrp1.* destructor


class source_s(usrp_common):
    def __init__(self, which=0, decim_rate=64, nchan=1, mux=0x32103210, mode=0,
                 fusb_block_size=0, fusb_nblocks=0,
                 fpga_filename="", firmware_filename=""):
        _ensure_rev2(which)
        self._u = usrp1.source_s(which, decim_rate, nchan, mux, mode,
                                 fusb_block_size, fusb_nblocks,
                                 fpga_filename, firmware_filename)
        # Add the db attribute, which contains a 2-tuple of tuples of daughterboard classes
        self.db = (db_instantiator.instantiate(self._u, 0),
                   db_instantiator.instantiate(self._u, 1))
        usrp_common.__init__(self)

    def __del__(self):
        self.db = None          # will fire d'board destructors
        self._u = None          # will fire usrp1.* destructor
        

# ------------------------------------------------------------------------
#                               utilities
# ------------------------------------------------------------------------

def determine_rx_mux_value(u, subdev_spec):
    """
    Determine appropriate Rx mux value as a function of the subdevice choosen and the
    characteristics of the respective daughterboard.

    @param u:           instance of USRP source
    @param subdev_spec: return value from subdev option parser.  
    @type  subdev_spec: (side, subdev), where side is 0 or 1 and subdev is 0 or 1
    @returns:           the Rx mux value
    """
    # Figure out which A/D's to connect to the DDC.
    #
    # Each daughterboard consists of 1 or 2 subdevices.  (At this time,
    # all but the Basic Rx have a single subdevice.  The Basic Rx
    # has two independent channels, treated as separate subdevices).
    # subdevice 0 of a daughterboard may use 1 or 2 A/D's.  We determine this
    # by checking the is_quadrature() method.  If subdevice 0 uses only a single
    # A/D, it's possible that the daughterboard has a second subdevice, subdevice 1,
    # and it uses the second A/D.
    #
    # If the card uses only a single A/D, we wire a zero into the DDC Q input.
    #
    # (side, 0) says connect only the A/D's used by subdevice 0 to the DDC.
    # (side, 1) says connect only the A/D's used by subdevice 1 to the DDC.
    #

    side = subdev_spec[0]  # side A = 0, side B = 1

    if not(side in (0, 1)):
        raise ValueError, "Invalid subdev_spec: %r:" % (subdev_spec,)

    db = u.db[side]        # This is a tuple of length 1 or 2 containing the subdevice
                           #   classes for the selected side.
    
    # compute bitmasks of used A/D's
    
    if db[0].is_quadrature():
        subdev0_uses = 0x3              # uses A/D 0 and 1
    else:
        subdev0_uses = 0x1              # uses A/D 0 only

    if len(db) > 1:
        subdev1_uses = 0x2              # uses A/D 1 only
    else:
        subdev1_uses = 0x0              # uses no A/D (doesn't exist)

    if subdev_spec[1] == 0:
        uses = subdev0_uses
    elif subdev_spec[1] == 1:
        uses = subdev1_uses
    else:
        raise ValueError, "Invalid subdev_spec: %r: " % (subdev_spec,)
    
    if uses == 0:
        raise RuntimeError, "Daughterboard doesn't have a subdevice 1: %r: " % (subdev_spec,)

    swap_iq = db[0].i_and_q_swapped()
    
    truth_table = {
        # (side, uses, swap_iq) : mux_val
        (0, 0x1, False) : 0xf0f0f0f0,
        (0, 0x2, False) : 0xf0f0f0f1,
        (0, 0x3, False) : 0x00000010,
        (0, 0x3, True)  : 0x00000001,
        (1, 0x1, False) : 0xf0f0f0f2,
        (1, 0x2, False) : 0xf0f0f0f3,
        (1, 0x3, False) : 0x00000032,
        (1, 0x3, True)  : 0x00000023
        }

    return gru.hexint(truth_table[(side, uses, swap_iq)])


def determine_tx_mux_value(u, subdev_spec):
    """
    Determine appropriate Tx mux value as a function of the subdevice choosen.

    @param u:           instance of USRP source
    @param subdev_spec: return value from subdev option parser.  
    @type  subdev_spec: (side, subdev), where side is 0 or 1 and subdev is 0
    @returns:           the Rx mux value
    """
    # This is simpler than the rx case.  Either you want to talk
    # to side A or side B.  If you want to talk to both sides at once,
    # determine the value manually.

    side = subdev_spec[0]  # side A = 0, side B = 1

    if not(side in (0, 1)):
        raise ValueError, "Invalid subdev_spec: %r:" % (subdev_spec,)

    return gru.hexint([0x0098, 0x9800][side])


def selected_subdev(u, subdev_spec):
    """
    Return the user specified daughterboard subdevice.

    @param u: an instance of usrp.source_* or usrp.sink_*
    @param subdev_spec: return value from subdev option parser.  
    @type  subdev_spec: (side, subdev), where side is 0 or 1 and subdev is 0 or 1
    @returns: an instance derived from db_base
    """
    side, subdev = subdev_spec
    return u.db[side][subdev]


def calc_dxc_freq(target_freq, baseband_freq, fs):
    """
    Calculate the frequency to use for setting the digital up or down converter.
    
    @param target_freq: desired RF frequency (Hz)
    @type  target_freq: number
    @param baseband_freq: the RF frequency that corresponds to DC in the IF.
    @type  baseband_freq: number
    @param fs: converter sample rate
    @type  fs: number
    
    @returns: 2-tuple (ddc_freq, inverted) where ddc_freq is the value
       for the ddc and inverted is True if we're operating in an inverted
       Nyquist zone.
    """

    delta = target_freq - baseband_freq

    if delta >= 0:
        while delta > fs:
            delta -= fs
        if delta <= fs/2:
            return (-delta, False)        # non-inverted region
        else:
            return (delta - fs, True)     # inverted region
    else:
        while delta < -fs:
            delta += fs
        if delta >= -fs/2:
            return (-delta, False)        # non-inverted region
        else:
            return (delta + fs, True)     # inverted region
    
    
# ------------------------------------------------------------------------
#                              Utilities
# ------------------------------------------------------------------------

def pick_tx_subdevice(u):
    """
    The user didn't specify a tx subdevice on the command line.
    Try for one of these, in order: FLEX_400, FLEX_900, FLEX_1200, FLEX_2400,
    BASIC_TX, whatever's on side A.

    @return a subdev_spec
    """
    return pick_subdev(u, (usrp_dbid.FLEX_400_TX,
                           usrp_dbid.FLEX_900_TX,
                           usrp_dbid.FLEX_1200_TX,
                           usrp_dbid.FLEX_2400_TX,
                           usrp_dbid.BASIC_TX))

def pick_rx_subdevice(u):
    """
    The user didn't specify an rx subdevice on the command line.
    Try for one of these, in order: FLEX_400, FLEX_900, FLEX_1200, FLEX_2400,
    TV_RX, DBS_RX, BASIC_RX, whatever's on side A.

    @return a subdev_spec
    """
    return pick_subdev(u, (usrp_dbid.FLEX_400_RX,
                           usrp_dbid.FLEX_900_RX,
                           usrp_dbid.FLEX_1200_RX,
                           usrp_dbid.FLEX_2400_RX,
                           usrp_dbid.TV_RX,
                           usrp_dbid.TV_RX_REV_2,
                           usrp_dbid.DBS_RX,
                           usrp_dbid.DBS_RX_REV_2_1,
                           usrp_dbid.BASIC_RX))

def pick_subdev(u, candidates):
    """
    @param u:          usrp instance
    @param candidates: list of dbids
    @returns: subdev specification
    """
    db0 = u.db[0][0].dbid()
    db1 = u.db[1][0].dbid()
    for c in candidates:
        if c == db0: return (0, 0)
        if c == db1: return (1, 0)
    if db0 >= 0:
        return (0, 0)
    if db1 >= 0:
        return (1, 0)
    raise RuntimeError, "No suitable daughterboard found!"

