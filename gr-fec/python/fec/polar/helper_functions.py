#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
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

import numpy as np


def is_power_of_two(num):
    if type(num) != int:
        return False  # make sure we only compute integers.
    return num != 0 and ((num & (num - 1)) == 0)


def bit_reverse(value, n):
    # is this really missing in NumPy???
    seq = np.int(value)
    rev = np.int(0)
    rmask = np.int(1)
    lmask = np.int(2 ** (n - 1))
    for i in range(n // 2):
        shiftval = n - 1 - (i * 2)
        rshift = np.left_shift(np.bitwise_and(seq, rmask), shiftval)
        lshift = np.right_shift(np.bitwise_and(seq, lmask), shiftval)
        rev = np.bitwise_or(rev, rshift)
        rev = np.bitwise_or(rev, lshift)
        rmask = np.left_shift(rmask, 1)
        lmask = np.right_shift(lmask, 1)
    if not n % 2 == 0:
        rev = np.bitwise_or(rev, np.bitwise_and(seq, rmask))
    return rev


def bit_reverse_vector(vec, n):
    return np.array([bit_reverse(e, n) for e in vec], dtype=vec.dtype)


def unpack_byte(byte, nactive):
    if np.amin(byte) < 0 or np.amax(byte) > 255:
        return None
    if not byte.dtype == np.uint8:
        byte = byte.astype(np.uint8)
    if nactive == 0:
        return np.array([], dtype=np.uint8)
    return np.unpackbits(byte)[-nactive:]


def pack_byte(bits):
    if len(bits) == 0:
        return 0
    if np.amin(bits) < 0 or np.amax(bits) > 1:  # only '1' and '0' in bits array allowed!
        return None
    bits = np.concatenate((np.zeros(8 - len(bits), dtype=np.uint8), bits))
    res = np.packbits(bits)[0]
    return res


def main():
    print 'helper functions'

    for i in range(8):
        print(i, 'is power of 2: ', is_power_of_two(i))
    n = 2 ** 6
    k = n // 2
    eta = 0.3


if __name__ == '__main__':
    main()
