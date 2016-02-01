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

# for dev
from encoder import PolarEncoder
from matplotlib import pyplot as plt


class PolarDecoder(PolarCommon):
    def __init__(self, n, k, frozen_bit_position, frozenbits=None):
        PolarCommon.__init__(self, n, k, frozen_bit_position, frozenbits)

        self.error_probability = 0.1  # this is kind of a dummy value. usually chosen individually.
        self.lrs = ((1 - self.error_probability) / self.error_probability, self.error_probability / (1 - self.error_probability))
        self.llrs = np.log(self.lrs)

    def _llr_bit(self, bit):
        return self.llrs[bit]

    def _llr_odd(self, la, lb):
        # this functions uses the min-sum approximation
        # exact formula: np.log((np.exp(la + lb) + 1) / (np.exp(la) + np.exp(lb)))
        return np.sign(la) * np.sign(lb) * np.minimum(np.abs(la), np.abs(lb))

    _f_vals = np.array((1.0, -1.0), dtype=float)

    def _llr_even(self, la, lb, f):
        return (la * self._f_vals[f]) + lb

    def _llr_bit_decision(self, llr):
        if llr < 0.0:
            ui = int(1)
        else:
            ui = int(0)
        return ui

    def _retrieve_bit_from_llr(self, lr, pos):
        f_index = np.where(self.frozen_bit_position == pos)[0]
        if not f_index.size == 0:
            ui = self.frozenbits[f_index][0]
        else:
            ui = self._llr_bit_decision(lr)
        return ui

    def _lr_bit(self, bit):
        return self.lrs[bit]

    def _lr_odd(self, la, lb):
        # la is upper branch and lb is lower branch
        return (la * lb + 1) / (la + lb)

    def _lr_even(self, la, lb, f):
        # la is upper branch and lb is lower branch, f is last decoded bit.
        return (la ** (1 - (2 * f))) * lb

    def _lr_bit_decision(self, lr):
        if lr < 1:
            return int(1)
        return int(0)

    def _get_even_indices_values(self, u_hat):
        # looks like overkill for some indexing, but zero and one based indexing mix-up gives you haedaches.
        return u_hat[1::2]

    def _get_odd_indices_values(self, u_hat):
        return u_hat[0::2]

    def _calculate_lrs(self, y, u):
        ue = self._get_even_indices_values(u)
        uo = self._get_odd_indices_values(u)
        ya = y[0:y.size//2]
        yb = y[(y.size//2):]
        la = self._lr_decision_element(ya, (ue + uo) % 2)
        lb = self._lr_decision_element(yb, ue)
        return la, lb

    def _lr_decision_element(self, y, u):
        if y.size == 1:
            return self._llr_bit(y[0])
        if u.size % 2 == 0:  # use odd branch formula
            la, lb = self._calculate_lrs(y, u)
            return self._llr_odd(la, lb)
        else:
            ui = u[-1]
            la, lb = self._calculate_lrs(y, u[0:-1])
            return self._llr_even(la, lb, ui)

    def _retrieve_bit_from_lr(self, lr, pos):
        f_index = np.where(self.frozen_bit_position == pos)[0]
        if not f_index.size == 0:
            ui = self.frozenbits[f_index][0]
        else:
            ui = self._lr_bit_decision(lr)
        return ui

    def _lr_sc_decoder(self, y):
        # this is the standard SC decoder as derived from the formulas. It sticks to natural bit order.
        u = np.array([], dtype=int)
        for i in range(y.size):
            lr = self._lr_decision_element(y, u)
            ui = self._retrieve_bit_from_llr(lr, i)
            u = np.append(u, ui)
        return u

    def _llr_retrieve_bit(self, llr, pos):
        f_index = np.where(self.frozen_bit_position == pos)[0]
        if not f_index.size == 0:
            ui = self.frozenbits[f_index][0]
        else:
            ui = self._llr_bit_decision(llr)
        return ui

    def _butterfly_decode_bits(self, pos, graph, u):
        bit_num = u.size
        llr = graph[pos][0]
        ui = self._llr_retrieve_bit(llr, bit_num)
        # ui = self._llr_bit_decision(llr)
        u = np.append(u, ui)
        lower_right = pos + (self.N // 2)
        la = graph[pos][1]
        lb = graph[lower_right][1]
        graph[lower_right][0] = self._llr_even(la, lb, ui)
        llr = graph[lower_right][0]
        # ui = self._llr_bit_decision(llr)
        ui = self._llr_retrieve_bit(llr, u.size)
        u = np.append(u, ui)
        return graph, u

    def _lr_sc_decoder_efficient(self, y):
        graph = np.full((self.N, self.power + 1), np.NaN, dtype=float)
        for i in range(self.N):
            graph[i][self.power] = self._llr_bit(y[i])
        decode_order = self._vector_bit_reversed(np.arange(self.N), self.power)
        decode_order = np.delete(decode_order, np.where(decode_order >= self.N // 2))
        u = np.array([], dtype=int)
        for pos in decode_order:
            graph = self._butterfly(pos, 0, graph, u)
            graph, u = self._butterfly_decode_bits(pos, graph, u)
        return u

    def _stop_propagation(self, bf_entry_row, stage):
        # calculate break condition
        modulus = 2 ** (self.power - stage)
        # stage_size = self.N // (2 ** stage)
        # half_stage_size = stage_size // 2
        half_stage_size = self.N // (2 ** (stage + 1))
        stage_pos = bf_entry_row % modulus
        return stage_pos >= half_stage_size

    def _butterfly(self, bf_entry_row, stage, graph, u):
        if not self.power > stage:
            return graph

        if self._stop_propagation(bf_entry_row, stage):
            upper_right = bf_entry_row - self.N // (2 ** (stage + 1))
            la = graph[upper_right][stage + 1]
            lb = graph[bf_entry_row][stage + 1]
            ui = u[-1]
            graph[bf_entry_row][stage] = self._llr_even(la, lb, ui)
            return graph

        # activate right side butterflies
        u_even = self._get_even_indices_values(u)
        u_odd = self._get_odd_indices_values(u)
        graph = self._butterfly(bf_entry_row, stage + 1, graph, (u_even + u_odd) % 2)
        lower_right = bf_entry_row + self.N // (2 ** (stage + 1))
        graph = self._butterfly(lower_right, stage + 1, graph, u_even)

        la = graph[bf_entry_row][stage + 1]
        lb = graph[lower_right][stage + 1]
        graph[bf_entry_row][stage] = self._llr_odd(la, lb)
        return graph

    def decode(self, data, is_packed=False):
        if not len(data) == self.N:
            raise ValueError("len(data)={0} is not equal to n={1}!".format(len(data), self.N))
        if is_packed:
            data = np.unpackbits(data)
        data = self._lr_sc_decoder_efficient(data)
        data = self._extract_info_bits(data)
        if is_packed:
            data = np.packbits(data)
        return data

    def _extract_info_bits_reversed(self, y):
        info_bit_positions_reversed = self._vector_bit_reversed(self.info_bit_position, self.power)
        return y[info_bit_positions_reversed]

    def decode_systematic(self, data):
        if not len(data) == self.N:
            raise ValueError("len(data)={0} is not equal to n={1}!".format(len(data), self.N))
        # data = self._reverse_bits(data)
        data = self._lr_sc_decoder_efficient(data)
        data = self._encode_natural_order(data)
        data = self._extract_info_bits_reversed(data)
        return data


def test_systematic_decoder():
    ntests = 1000
    n = 16
    k = 8
    frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    encoder = PolarEncoder(n, k, frozenbitposition)
    decoder = PolarDecoder(n, k, frozenbitposition)
    for i in range(ntests):
        bits = np.random.randint(2, size=k)
        y = encoder.encode_systematic(bits)
        u_hat = decoder.decode_systematic(y)
        assert (bits == u_hat).all()


def test_reverse_enc_dec():
    n = 16
    k = 8
    frozenbits = np.zeros(n - k)
    frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    bits = np.random.randint(2, size=k)
    encoder = PolarEncoder(n, k, frozenbitposition, frozenbits)
    decoder = PolarDecoder(n, k, frozenbitposition, frozenbits)
    encoded = encoder.encode(bits)
    print 'encoded:', encoded
    rx = decoder.decode(encoded)
    print 'bits:', bits
    print 'rx  :', rx
    print (bits == rx).all()


def compare_decoder_impls():
    print '\nthis is decoder test'
    n = 8
    k = 4
    frozenbits = np.zeros(n - k)
    # frozenbitposition16 = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    frozenbitposition = np.array((0, 1, 2, 4), dtype=int)
    bits = np.random.randint(2, size=k)
    print 'bits:', bits
    encoder = PolarEncoder(n, k, frozenbitposition, frozenbits)
    decoder = PolarDecoder(n, k, frozenbitposition, frozenbits)
    encoded = encoder.encode(bits)
    print 'encoded:', encoded
    rx_st = decoder._lr_sc_decoder(encoded)
    rx_eff = decoder._lr_sc_decoder_efficient(encoded)
    print 'standard :', rx_st
    print 'efficient:', rx_eff
    print (rx_st == rx_eff).all()


def main():
    # power = 3
    # n = 2 ** power
    # k = 4
    # frozenbits = np.zeros(n - k, dtype=int)
    # frozenbitposition = np.array((0, 1, 2, 4), dtype=int)
    # frozenbitposition4 = np.array((0, 1), dtype=int)
    #
    #
    # encoder = PolarEncoder(n, k, frozenbitposition, frozenbits)
    # decoder = PolarDecoder(n, k, frozenbitposition, frozenbits)
    #
    # bits = np.ones(k, dtype=int)
    # print "bits: ", bits
    # evec = encoder.encode(bits)
    # print "froz: ", encoder._insert_frozen_bits(bits)
    # print "evec: ", evec
    #
    # evec[1] = 0
    # deced = decoder._lr_sc_decoder(evec)
    # print 'SC decoded:', deced
    #
    # test_reverse_enc_dec()
    # compare_decoder_impls()

    test_systematic_decoder()


if __name__ == '__main__':
    main()
