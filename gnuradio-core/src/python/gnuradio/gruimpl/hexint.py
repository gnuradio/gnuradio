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

def hexint(mask):
  """
  Convert unsigned masks into signed ints.

  This allows us to use hex constants like 0xf0f0f0f2 when talking to
  our hardware and not get screwed by them getting treated as python
  longs.
  """
  if mask >= 2**31:
     return int(mask-2**32)
  return mask

def hexshort(mask):
  """
  Convert unsigned masks into signed shorts.

  This allows us to use hex constants like 0x8000 when talking to
  our hardware and not get screwed by them getting treated as python
  longs.
  """
  if mask >= 2**15:
    return int(mask-2**16)
  return mask
