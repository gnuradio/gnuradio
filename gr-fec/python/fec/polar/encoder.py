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
from common import PolarCommon
import helper_functions as hf


class PolarEncoder(PolarCommon):
    def __init__(self, n, k, frozen_bit_position, frozenbits=None):
        PolarCommon.__init__(self, n, k, frozen_bit_position, frozenbits)
        self.G = hf.get_Fn(n)

    def get_gn(self):
        return self.G

    def _prepare_input_data(self, vec):
        vec = self._insert_frozen_bits(vec)
        vec = self._reverse_bits(vec)
        return vec

    def _encode_matrix(self, data):
        data = np.dot(data, self.G) % 2
        data = data.astype(dtype=int)
        return data

    def encode(self, data, is_packed=False):
        if not len(data) == self.K:
            raise ValueError("len(data)={0} is not equal to k={1}!".format(len(data), self.K))
        if is_packed:
            data = np.unpackbits(data)
        if np.max(data) > 1 or np.min(data) < 0:
            raise ValueError("can only encode bits!")
        data = self._prepare_input_data(data)
        data = self._encode_efficient(data)
        if is_packed:
            data = np.packbits(data)
        return data

    def encode_systematic(self, data):
        if not len(data) == self.K:
            raise ValueError("len(data)={0} is not equal to k={1}!".format(len(data), self.K))
        if np.max(data) > 1 or np.min(data) < 0:
            raise ValueError("can only encode bits!")

        d = self._insert_frozen_bits(data)
        d = self._encode_natural_order(d)
        d = self._reverse_bits(d)
        d[self.frozen_bit_position] = 0
        d = self._encode_natural_order(d)
        # d = self._reverse_bits(d) # for more accuracy, do another bit-reversal. or don't for computational simplicity.
        return d


def test_systematic_encoder(encoder, ntests, k):
    for n in range(ntests):
        bits = np.random.randint(2, size=k)
        x = encoder.encode_systematic(bits)
        x = encoder._reverse_bits(x)
        u_hat = encoder._extract_info_bits(x)

        assert (bits == u_hat).all()
        # print((bits == u_hat).all())


def compare_results(encoder, ntests, k):
    for n in range(ntests):
        bits = np.random.randint(2, size=k)
        preped = encoder._prepare_input_data(bits)
        menc = encoder._encode_matrix(preped)
        fenc = encoder._encode_efficient(preped)
        if (menc == fenc).all() == False:
            return False
    return True


def test_pseudo_rate_1_encoder(encoder, ntests, k):
    for n in range(ntests):
        bits = np.random.randint(2, size=k)
        u = encoder._prepare_input_data(bits)
        fenc = encoder._encode_efficient(u)
        u_hat = encoder._encode_efficient(fenc)
        if not (u_hat == u).all():
            print('rate-1 encoder/decoder failed')
            print u
            print u_hat
            return False
    return True


def test_encoder_impls():
    print('Compare encoder implementations, matrix vs. efficient')
    ntests = 1000
    n = 16
    k = 8
    # frozenbits = np.zeros(n - k)
    # frozenbitposition8 = np.array((0, 1, 2, 4), dtype=int)  # keep it!
    frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    encoder = PolarEncoder(n, k, frozenbitposition)  #, frozenbits)
    print 'result:', compare_results(encoder, ntests, k)

    print('Test rate-1 encoder/decoder chain results')
    r1_test = test_pseudo_rate_1_encoder(encoder, ntests, k)
    print 'Test rate-1 encoder/decoder:', r1_test
    test_systematic_encoder(encoder, ntests, k)


def main():
    test_encoder_impls()


if __name__ == '__main__':
    main()
