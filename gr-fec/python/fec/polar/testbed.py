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
from encoder import PolarEncoder
from decoder import PolarDecoder

import matplotlib.pyplot as plt


def get_frozen_bit_position():
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 16, 17, 18, 20, 24), dtype=int)
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    frozenbitposition = np.load('frozen_bit_positions_n256_k128_p0.11.npy').flatten()
    print(frozenbitposition)
    return frozenbitposition


def test_enc_dec_chain():
    ntests = 100
    n = 256
    k = n // 2
    frozenbits = np.zeros(n - k)
    frozenbitposition = get_frozen_bit_position()
    for i in range(ntests):
        bits = np.random.randint(2, size=k)
        encoder = PolarEncoder(n, k, frozenbitposition, frozenbits)
        decoder = PolarDecoder(n, k, frozenbitposition, frozenbits)
        encoded = encoder.encode(bits)
        rx = decoder.decode(encoded)
        if not is_equal(bits, rx):
            raise ValueError('Test #', i, 'failed, input and output differ', bits, '!=', rx)
            return


def is_equal(first, second):
    if not (first == second).all():
        result = first == second
        for i in range(len(result)):
            print '{0:4}: {1:2} == {2:1} = {3}'.format(i, first[i], second[i], result[i])
        return False
    return True


def exact_value(la, lb):
    return np.log((np.exp(la + lb) + 1) / (np.exp(la) + np.exp(lb)))


def approx_value(la, lb):
    return np.sign(la) * np.sign(lb) * np.minimum(np.abs(la), np.abs(lb))


def test_1024_rate_1_code():
    # effectively a Monte-Carlo simulation for channel polarization.
    ntests = 10000
    n = 256
    k = n
    transition_prob = 0.11
    num_transitions = int(k * transition_prob)
    frozenbits = np.zeros(n - k)
    frozenbitposition = np.array((), dtype=int)
    encoder = PolarEncoder(n, k, frozenbitposition, frozenbits)
    decoder = PolarDecoder(n, k, frozenbitposition, frozenbits)

    channel_counter = np.zeros(k)
    possible_indices = np.arange(n, dtype=int)
    for i in range(ntests):
        bits = np.random.randint(2, size=k)
        tx = encoder.encode(bits)
        np.random.shuffle(possible_indices)
        tx[possible_indices[0:num_transitions]] = (tx[possible_indices[0:num_transitions]] + 1) % 2
        rx = tx
        recv = decoder.decode(rx)
        channel_counter += (bits == recv)

    print channel_counter
    print(np.min(channel_counter), np.max(channel_counter))

    np.save('channel_counter_' + str(ntests) + '.npy', channel_counter)


def find_good_indices(res, nindices):
    channel_counter = np.copy(res)
    good_indices = np.zeros(channel_counter.size)

    for i in range(nindices):
        idx = np.argmax(channel_counter)
        good_indices[idx] = 1
        channel_counter[idx] = 0
    return good_indices


def channel_analysis():
    ntests = 10000
    filename = 'channel_counter_' + str(ntests) + '.npy'
    channel_counter = np.load(filename)
    print(np.min(channel_counter), np.max(channel_counter))
    channel_counter[0] = np.min(channel_counter)
    good_indices = find_good_indices(channel_counter, channel_counter.size // 2)
    frozen_bit_positions = np.where(good_indices > 0)
    print(frozen_bit_positions)
    np.save('frozen_bit_positions_n256_k128_p0.11.npy', frozen_bit_positions)
    good_indices *= 2000
    good_indices += 4000


    plt.plot(channel_counter)
    plt.plot(good_indices)
    plt.show()

def main():
    # n = 16
    # k = 8
    # frozenbits = np.zeros(n - k)
    # frozenbitposition8 = np.array((0, 1, 2, 4), dtype=int)
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    # print frozenbitposition

    test_enc_dec_chain()

    # test_1024_rate_1_code()

    # channel_analysis()

if __name__ == '__main__':
    main()