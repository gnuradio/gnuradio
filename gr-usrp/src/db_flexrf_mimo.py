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

from gnuradio import usrp1
import time,math

from usrpm import usrp_dbid
import db_base
import db_instantiator
from usrpm.usrp_fpga_regs import *
from db_flexrf import *

# self._u.fpga_master_clock_freq()

# MIMO Classes
class db_flexrf_2400_tx_mimo_a(db_flexrf_2400_tx):
    def __init__(self, usrp, which):
        db_flexrf_2400_tx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16

class db_flexrf_2400_rx_mimo_a(db_flexrf_2400_rx):
    def __init__(self, usrp, which):
        db_flexrf_2400_rx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_2400_tx_mimo_b(db_flexrf_2400_tx):
    def __init__(self, usrp, which):
        db_flexrf_2400_tx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_2400_rx_mimo_b(db_flexrf_2400_rx):
    def __init__(self, usrp, which):
        db_flexrf_2400_rx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
class db_flexrf_1800_tx_mimo_a(db_flexrf_1800_tx):
    def __init__(self, usrp, which):
        db_flexrf_1800_tx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16

class db_flexrf_1800_rx_mimo_a(db_flexrf_1800_rx):
    def __init__(self, usrp, which):
        db_flexrf_1800_rx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_1800_tx_mimo_b(db_flexrf_1800_tx):
    def __init__(self, usrp, which):
        db_flexrf_1800_tx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_1800_rx_mimo_b(db_flexrf_1800_rx):
    def __init__(self, usrp, which):
        db_flexrf_1800_rx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_1200_tx_mimo_a(db_flexrf_1200_tx):
    def __init__(self, usrp, which):
        db_flexrf_1200_tx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16

class db_flexrf_1200_rx_mimo_a(db_flexrf_1200_rx):
    def __init__(self, usrp, which):
        db_flexrf_1200_rx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_1200_tx_mimo_b(db_flexrf_1200_tx):
    def __init__(self, usrp, which):
        db_flexrf_1200_tx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_1200_rx_mimo_b(db_flexrf_1200_rx):
    def __init__(self, usrp, which):
        db_flexrf_1200_rx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_900_tx_mimo_a(db_flexrf_900_tx):
    def __init__(self, usrp, which):
        db_flexrf_900_tx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_900_rx_mimo_a(db_flexrf_900_rx):
    def __init__(self, usrp, which):
        db_flexrf_900_rx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_900_tx_mimo_b(db_flexrf_900_tx):
    def __init__(self, usrp, which):
        db_flexrf_900_tx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
class db_flexrf_900_rx_mimo_b(db_flexrf_900_rx):
    def __init__(self, usrp, which):
        db_flexrf_900_rx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1

class db_flexrf_400_tx_mimo_a(db_flexrf_400_tx):
    def __init__(self, usrp, which):
        db_flexrf_400_tx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_400_rx_mimo_a(db_flexrf_400_rx):
    def __init__(self, usrp, which):
        db_flexrf_400_rx.__init__(self, usrp, which)
        self._enable_refclk(True)
        self.R_DIV = 1

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 16
    
class db_flexrf_400_tx_mimo_b(db_flexrf_400_tx):
    def __init__(self, usrp, which):
        db_flexrf_400_tx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
class db_flexrf_400_rx_mimo_b(db_flexrf_400_rx):
    def __init__(self, usrp, which):
        db_flexrf_400_rx.__init__(self, usrp, which)
        self.R_DIV = 16

    def _refclk_divisor(self):
        """
        Return value to stick in REFCLK_DIVISOR register
        """
        return 1
    
# hook these daughterboard classes into the auto-instantiation framework
db_instantiator.add(usrp_dbid.FLEX_2400_TX_MIMO_A, lambda usrp, which : (db_flexrf_2400_tx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_2400_RX_MIMO_A, lambda usrp, which : (db_flexrf_2400_rx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_TX_MIMO_A, lambda usrp, which : (db_flexrf_1800_tx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_RX_MIMO_A, lambda usrp, which : (db_flexrf_1800_rx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_TX_MIMO_A, lambda usrp, which : (db_flexrf_1200_tx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_RX_MIMO_A, lambda usrp, which : (db_flexrf_1200_rx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_TX_MIMO_A,  lambda usrp, which : (db_flexrf_900_tx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_RX_MIMO_A,  lambda usrp, which : (db_flexrf_900_rx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_TX_MIMO_A,  lambda usrp, which : (db_flexrf_400_tx_mimo_a(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_RX_MIMO_A,  lambda usrp, which : (db_flexrf_400_rx_mimo_a(usrp, which),))

db_instantiator.add(usrp_dbid.FLEX_2400_TX_MIMO_B, lambda usrp, which : (db_flexrf_2400_tx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_2400_RX_MIMO_B, lambda usrp, which : (db_flexrf_2400_rx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_TX_MIMO_B, lambda usrp, which : (db_flexrf_1800_tx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1800_RX_MIMO_B, lambda usrp, which : (db_flexrf_1800_rx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_TX_MIMO_B, lambda usrp, which : (db_flexrf_1200_tx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_1200_RX_MIMO_B, lambda usrp, which : (db_flexrf_1200_rx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_TX_MIMO_B,  lambda usrp, which : (db_flexrf_900_tx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_900_RX_MIMO_B,  lambda usrp, which : (db_flexrf_900_rx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_TX_MIMO_B,  lambda usrp, which : (db_flexrf_400_tx_mimo_b(usrp, which),))
db_instantiator.add(usrp_dbid.FLEX_400_RX_MIMO_B,  lambda usrp, which : (db_flexrf_400_rx_mimo_b(usrp, which),))

