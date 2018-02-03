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
import time, sys
import copy


def bsc_channel(p):
    '''
    binary symmetric channel (BSC)
    output alphabet Y = {0, 1} and
    W(0|0) = W(1|1) and W(1|0) = W(0|1)

    this function returns a prob's vector for a BSC
    p denotes an erroneous transition
    '''
    if not (p >= 0.0 and p <= 1.0):
        print "given p is out of range!"
        return np.array([], dtype=float)

    # 0 -> 0, 0 -> 1, 1 -> 0, 1 -> 1
    W = np.array([[1 - p, p], [p, 1 - p]], dtype=float)
    return W


def power_of_2_int(num):
    return int(np.log2(num))


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


def get_Bn(n):
    # this is a bit reversal matrix.
    lw = power_of_2_int(n)  # number of used bits
    indexes = [bit_reverse(i, lw) for i in range(n)]
    Bn = np.zeros((n, n), type(n))
    for i, index in enumerate(indexes):
        Bn[i][index] = 1
    return Bn


def get_Fn(n):
    # this matrix defines the actual channel combining.
    if n == 1:
        return np.array([1, ])
    nump = power_of_2_int(n) - 1  # number of Kronecker products to calculate
    F2 = np.array([[1, 0], [1, 1]], np.int)
    Fn = F2
    for i in range(nump):
        Fn = np.kron(Fn, F2)
    return Fn


def get_Gn(n):
    # this matrix is called generator matrix
    if not is_power_of_two(n):
        print "invalid input"
        return None
    if n == 1:
        return np.array([1, ])
    Bn = get_Bn(n)
    Fn = get_Fn(n)
    Gn = np.dot(Bn, Fn)
    return Gn


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


def show_progress_bar(ndone, ntotal):
    nchars = 50

    fract = (1. * ndone / ntotal)
    percentage = 100. * fract
    ndone_chars = int(nchars * fract)
    nundone_chars = nchars - ndone_chars
    sys.stdout.write('\r[{0}{1}] {2:5.2f}% ({3} / {4})'.format('=' * ndone_chars, ' ' * nundone_chars, percentage, ndone, ntotal))



def mutual_information(w):
    '''
    calculate mutual information I(W)
    I(W) = sum over y e Y ( sum over x e X ( ... ) )
    .5 W(y|x) log frac { W(y|x) }{ .5 W(y|0) + .5 W(y|1) }
    '''
    ydim, xdim = np.shape(w)
    i = 0.0
    for y in range(ydim):
        for x in range(xdim):
            v = w[y][x] * np.log2(w[y][x] / (0.5 * w[y][0] + 0.5 * w[y][1]))
            i += v
    i /= 2.0
    return i


def bhattacharyya_parameter(w):
    '''
    bhattacharyya parameter is a measure of similarity between two prob. distributions
    THEORY: sum over all y e Y for sqrt( W(y|0) * W(y|1) )
    Implementation:
    Numpy vector of dimension (2, mu//2)
    holds probabilities P(x|0), first vector for even, second for odd.
    '''
    dim = np.shape(w)
    if len(dim) != 2:
        raise ValueError

    if dim[0] > dim[1]:
        raise ValueError

    z = np.sum(np.sqrt(w[0] * w[1]))
    # need all
    return z


def main():
    print 'helper functions'

    for i in range(9):
        print(i, 'is power of 2: ', is_power_of_two(i))
    n = 6
    m = 2 ** n


    pos = np.arange(m)
    rev_pos = bit_reverse_vector(pos, n)
    print(pos)
    print(rev_pos)

    f = np.linspace(.01, .29, 10)
    e = np.linspace(.03, .31, 10)

    b = np.array([e, f])
    zp = bhattacharyya_parameter(b)
    print(zp)

    a = np.sum(np.sqrt(e * f))
    print(a)



if __name__ == '__main__':
    main()
