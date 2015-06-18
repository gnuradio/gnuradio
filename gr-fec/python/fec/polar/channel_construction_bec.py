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
import matplotlib.pyplot as plt

def bsc_channel(p):
    '''
    binary symmetric channel (BSC)
    output alphabet Y = {0, 1} and
    W(0|0) = W(1|1) and W(1|0) = W(0|1)

    this function returns a prob's vector for a BSC
    p denotes an erroneous transistion
    '''
    if not (p >= 0.0 and p <= 1.0):
        print "given p is out of range!"
        return np.array([], dtype=float)

    # 0 -> 0, 0 -> 1, 1 -> 0, 1 -> 1
    W = np.array([[1 - p, p], [p, 1 - p]], dtype=float)
    return W


def bec_channel(eta):
    '''
    binary erasure channel (BEC)
    for each y e Y
    W(y|0) * W(y|1) = 0 or W(y|0) = W(y|1)
    transistions are 1 -> 1 or 0 -> 0 or {0, 1} -> ? (erased symbol)
    '''

    # looks like BSC but should be interpreted differently.
    W = np.array((1 - eta, eta, 1 - eta), dtype=float)
    return W


def odd_rec(iwn):
    return iwn ** 2


def even_rec(iwn):
    return 2 * iwn - iwn ** 2


def calc_one_recursion(iw0):
    iw1 = np.zeros(2 * len(iw0))  # double values
    for i in range(len(iw0)):
        # careful indices screw you because paper is '1' based :(
        iw1[2 * i] = odd_rec(iw0[i])
        iw1[2 * i + 1] = even_rec(iw0[i])
    return iw1


def calculate_bec_channel_capacities(eta, n):
    iw = 1 - eta  # holds for BEC as stated in paper
    iw = np.array([iw, ], dtype=float)
    lw = int(np.log2(n))
    for i in range(lw):
        iw = calc_one_recursion(iw)
    return iw


def get_frozen_bit_indices_from_capacities(chan_caps, nfrozen):
    indexes = np.array([], dtype=int)
    while indexes.size < nfrozen:
        index = np.argmin(chan_caps)
        indexes = np.append(indexes, index)
        chan_caps[index] = 1.0
    return np.sort(indexes)


def get_bec_frozen_indices(nblock, kfrozen, eta):
    bec_caps = calculate_bec_channel_capacities(eta, nblock)
    positions = get_frozen_bit_indices_from_capacities(bec_caps, kfrozen)
    return positions


def bec_channel_contruction_tests():
    n = 2 ** 10
    k = n // 2
    eta = 0.3
    bec_capacities = calculate_bec_channel_capacities(eta, n)
    print(bec_capacities)

    frozenbits_position = get_frozen_bit_indices_from_capacities(bec_capacities, k)

    print('frozenbit_positions:')
    print(frozenbits_position)
    print('frozenbit_num:', frozenbits_position.size)


def main():
    print 'channel construction main'
    bec_channel_contruction_tests()


if __name__ == '__main__':
    main()
