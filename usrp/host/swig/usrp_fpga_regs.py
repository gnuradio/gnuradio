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

from usrpm import usrp_prims

# Copy everything that starts with FR_ or bmFR_ from the usrp_prims
# name space into our name space.  This is effectively a python binding for
# the contents of firmware/include/fpga_regs_common.h and fpga_regs_standard.h

for name in dir(usrp_prims):
    if name.startswith("FR_") or name.startswith("bmFR_"):
        globals()[name] = usrp_prims.__dict__[name]
