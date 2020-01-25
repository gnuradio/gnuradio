from __future__ import unicode_literals
#
# Copyright 2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
