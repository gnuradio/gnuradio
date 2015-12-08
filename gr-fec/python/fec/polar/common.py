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
from helper_functions import *

'''
PolarCommon holds value checks and common initializer code for both Encoder and Decoder.
'''


class PolarCommon:
    def __init__(self, n, k, frozen_bit_position, frozenbits=None):
        if not is_power_of_two(n):
            raise ValueError("n={0} is not a power of 2!".format(n))
        if frozenbits is None:
            frozenbits = np.zeros(n - k, dtype=np.int)
        if not len(frozenbits) == n - k:
            raise ValueError("len(frozenbits)={0} is not equal to n-k={1}!".format(len(frozenbits), n - k))
        if not frozenbits.dtype == np.int:
            frozenbits = frozenbits.astype(dtype=int)
        if not len(frozen_bit_position) == (n - k):
            raise ValueError("len(frozen_bit_position)={0} is not equal to n-k={1}!".format(len(frozen_bit_position), n - k))
        if not frozen_bit_position.dtype == np.int:
            frozen_bit_position = frozen_bit_position.astype(dtype=int)

        self.bit_reverse_positions = self._vector_bit_reversed(np.arange(n, dtype=int), int(np.log2(n)))
        self.N = n
        self.power = int(np.log2(self.N))
        self.K = k
        self.frozenbits = frozenbits
        self.frozen_bit_position = frozen_bit_position
        self.info_bit_position = np.delete(np.arange(self.N), self.frozen_bit_position)

    def _insert_frozen_bits(self, u):
        prototype = np.empty(self.N, dtype=int)
        prototype[self.frozen_bit_position] = self.frozenbits
        prototype[self.info_bit_position] = u
        return prototype

    def _extract_info_bits(self, y):
        return y[self.info_bit_position]

    def _reverse_bits(self, vec):
        return vec[self.bit_reverse_positions]

    def _vector_bit_reversed(self, vec, n):
        return bit_reverse_vector(vec, n)

    def _encode_efficient(self, vec):
        n_stages = self.power
        pos = np.arange(self.N, dtype=int)
        for i in range(n_stages):
            splitted = np.reshape(pos, (2 ** (i + 1), -1))
            upper_branch = splitted[0::2].flatten()
            lower_branch = splitted[1::2].flatten()
            vec[upper_branch] = (vec[upper_branch] + vec[lower_branch]) % 2
        return vec

    def _encode_natural_order(self, vec):
        # use this function. It reflects the encoding process implemented in VOLK.
        vec = vec[self.bit_reverse_positions]
        return self._encode_efficient(vec)

    def info_print(self):
        print "POLAR code ({0}, {1})".format(self.N, self.K)
