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
import helper_functions as hf


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


def calculate_bec_channel_capacities(eta, block_size):
    # compare [0, Arikan] eq. 6
    iw = 1 - eta  # holds for BEC as stated in paper
    iw = np.array([iw, ], dtype=float)
    lw = int(np.log2(block_size))
    for i in range(lw):
        iw = calc_one_recursion(iw)
    return iw


def calculate_z_parameters_one_recursion(z_params):
    z_next = np.zeros(2 * z_params.size)
    for i in range(z_params.size):
        z_sq = z_params[i] ** 2
        z_next[2 * i] = 2 * z_params[i] - z_sq
        z_next[2 * i + 1] = z_sq
    return z_next


def calculate_bec_channel_z_parameters(eta, block_size):
    # compare [0, Arikan] eq. 38
    block_power = hf.power_of_2_int(block_size)
    z_params = np.array([eta,], dtype=float)
    for block_size in range(block_power):
        z_params = calculate_z_parameters_one_recursion(z_params)
    return z_params


def design_snr_to_bec_eta(design_snr):
    s = 10. ** (design_snr / 10.)
    return np.exp(-s)


def bhattacharyya_bounds(design_snr, block_size):
    '''
    Harish Vangala, Emanuele Viterbo, Yi Hong: 'A Comparative Study of Polar Code Constructions for the AWGN Channel', 2015
    In this paper it is called Bhattacharyya bounds channel construction and is abbreviated PCC-0
    Best design SNR for block_size = 2048, R = 0.5, is 0dB.
    Compare with Arikan: 'Channel Polarization: A Method for Constructing Capacity-Achieving Codes for Symmetric Binary-Input Memoryless Channels.
    Proposition 5. inequalities turn into equalities for BEC channel. Otherwise they represent an upper bound.
    Also compare [0, Arikan] eq. 6 and 38
    For BEC that translates to capacity(i) = 1 - bhattacharyya(i)
    :return Z-parameters in natural bit-order. Choose according to desired rate.
    '''
    # minimum design snr = -1.5917 corresponds to BER = 0.5
    s = 10 ** (design_snr / 10)  # 'initial z parameter'.
    eta = np.exp(-s)
    return calculate_bec_channel_z_parameters(eta, block_size)


def main():
    print 'channel construction main'
    n = 4
    block_size = 2 ** n
    design_snr = 1.0
    eta = design_snr_to_bec_eta(design_snr)
    print(calculate_bec_channel_z_parameters(eta, block_size))
    print(calculate_bec_channel_capacities(eta, block_size))

if __name__ == '__main__':
    main()
