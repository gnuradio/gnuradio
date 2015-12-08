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


from encoder import PolarEncoder
from decoder import PolarDecoder
import channel_construction as cc
from helper_functions import *

import matplotlib.pyplot as plt


def get_frozen_bit_position():
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 16, 17, 18, 20, 24), dtype=int)
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    m = 256
    n_frozen = m // 2
    frozenbitposition = cc.get_frozen_bit_indices_from_z_parameters(cc.bhattacharyya_bounds(0.0, m), n_frozen)
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


def path_metric_exact(last_pm, llr, ui):
    return last_pm + np.log(1 + np.exp(-1. * llr * (1 - 2 * ui)))


def path_metric_approx(last_pm, llr, ui):
    if ui == int(.5 * (1 - np.sign(llr))):
        return last_pm
    return last_pm + np.abs(llr)


def calculate_path_metric_vector(metric, llrs, us):
    res = np.zeros(llrs.size)
    res[0] = metric(0, llrs[0], us[0])
    for i in range(1, llrs.size):
        res[i] = metric(res[i - 1], llrs[i], us[i])
    return res


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
    info_bit_positions = np.where(good_indices > 0)
    print(info_bit_positions)
    frozen_bit_positions = np.delete(np.arange(channel_counter.size), info_bit_positions)
    print(frozen_bit_positions)
    np.save('frozen_bit_positions_n256_k128_p0.11.npy', frozen_bit_positions)
    good_indices *= 2000
    good_indices += 4000

    plt.plot(channel_counter)
    plt.plot(good_indices)
    plt.show()


def merge_first_stage(init_mask):
    merged_frozen_mask = []
    for e in range(0, len(init_mask), 2):
        v = [init_mask[e]['value'][0], init_mask[e + 1]['value'][0]]
        s = init_mask[e]['size'] * 2
        if init_mask[e]['type'] == init_mask[e + 1]['type']:
            t = init_mask[e]['type']
            merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
        else:
            t = 'RPT'
            merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
    return merged_frozen_mask


def merge_second_stage(init_mask):
    merged_frozen_mask = []
    for e in range(0, len(init_mask), 2):
        if init_mask[e]['type'] == init_mask[e + 1]['type']:
            t = init_mask[e]['type']
            v = init_mask[e]['value']
            v.extend(init_mask[e + 1]['value'])
            s = init_mask[e]['size'] * 2
            merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
        elif init_mask[e]['type'] == 'ZERO' and init_mask[e + 1]['type'] == 'RPT':
            t = init_mask[e + 1]['type']
            v = init_mask[e]['value']
            v.extend(init_mask[e + 1]['value'])
            s = init_mask[e]['size'] * 2
            merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
        elif init_mask[e]['type'] == 'RPT' and init_mask[e + 1]['type'] == 'ONE':
            t = 'SPC'
            v = init_mask[e]['value']
            v.extend(init_mask[e + 1]['value'])
            s = init_mask[e]['size'] * 2
            merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
        else:
            merged_frozen_mask.append(init_mask[e])
            merged_frozen_mask.append(init_mask[e + 1])
    return merged_frozen_mask


def merge_stage_n(init_mask):
    merged_frozen_mask = []
    n_elems = len(init_mask) - (len(init_mask) % 2)
    for e in range(0, n_elems, 2):
        if init_mask[e]['size'] == init_mask[e + 1]['size']:
            if (init_mask[e]['type'] == 'ZERO' or init_mask[e]['type'] == 'ONE') and init_mask[e]['type'] == init_mask[e + 1]['type']:
                t = init_mask[e]['type']
                v = init_mask[e]['value']
                v.extend(init_mask[e + 1]['value'])
                s = init_mask[e]['size'] * 2
                merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
            elif init_mask[e]['type'] == 'ZERO' and init_mask[e + 1]['type'] == 'RPT':
                t = init_mask[e + 1]['type']
                v = init_mask[e]['value']
                v.extend(init_mask[e + 1]['value'])
                s = init_mask[e]['size'] * 2
                merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
            elif init_mask[e]['type'] == 'SPC' and init_mask[e + 1]['type'] == 'ONE':
                t = init_mask[e]['type']
                v = init_mask[e]['value']
                v.extend(init_mask[e + 1]['value'])
                s = init_mask[e]['size'] * 2
                merged_frozen_mask.append({'value': v, 'type': t, 'size': s})
            else:
                merged_frozen_mask.append(init_mask[e])
                merged_frozen_mask.append(init_mask[e + 1])
        else:
            merged_frozen_mask.append(init_mask[e])
            merged_frozen_mask.append(init_mask[e + 1])
    if n_elems < len(init_mask):
        merged_frozen_mask.append(init_mask[-1])
    return merged_frozen_mask


def print_decode_subframes(subframes):
    for e in subframes:
        print(e)


def find_decoder_subframes(frozen_mask):
    stages = power_of_2_int(len(frozen_mask))
    block_size = 2 ** stages

    lock_mask = np.zeros(block_size, dtype=int)
    sub_mask = []

    for e in frozen_mask:
        if e == 1:
            sub_mask.append(0)
        else:
            sub_mask.append(1)
    sub_mask = np.array(sub_mask, dtype=int)

    for s in range(0, stages):
        stage_size = 2 ** s
        mask = np.reshape(sub_mask, (-1, stage_size))
        lock = np.reshape(lock_mask, (-1, stage_size))
        for p in range(0, (block_size // stage_size) - 1, 2):
            l0 = lock[p]
            l1 = lock[p + 1]
            first = mask[p]
            second = mask[p + 1]
            print(l0, l1)
            print(first, second)
            if np.all(l0 == l1):
                for eq in range(2):
                    if np.all(first == eq) and np.all(second == eq):
                        mask[p].fill(eq)
                        mask[p + 1].fill(eq)
                        lock[p].fill(s)
                        lock[p + 1].fill(s)

                if np.all(first == 0) and np.all(second == 2):
                    mask[p].fill(2)
                    mask[p + 1].fill(2)
                    lock[p].fill(s)
                    lock[p + 1].fill(s)

                if np.all(first == 3) and np.all(second == 1):
                    mask[p].fill(3)
                    mask[p + 1].fill(3)
                    lock[p].fill(s)
                    lock[p + 1].fill(s)

            if s == 0 and np.all(first == 0) and np.all(second == 1):
                mask[p].fill(2)
                mask[p + 1].fill(2)
                lock[p].fill(s)
                lock[p + 1].fill(s)

            if s == 1 and np.all(first == 2) and np.all(second == 1):
                mask[p].fill(3)
                mask[p + 1].fill(3)
                lock[p].fill(s)
                lock[p + 1].fill(s)

        sub_mask = mask.flatten()
        lock_mask = lock.flatten()

    words = {0: 'ZERO', 1: 'ONE', 2: 'RPT', 3: 'SPC'}
    ll = lock_mask[0]
    sub_t = sub_mask[0]
    for i in range(len(frozen_mask)):
        v = frozen_mask[i]
        t = words[sub_mask[i]]
        l = lock_mask[i]
        # if i % 8 == 0:
        #     print
        if not l == ll or not sub_mask[i] == sub_t:
            print('--------------------------')
        ll = l
        sub_t = sub_mask[i]
        print('{0:4} lock {1:4} value: {2} in sub {3}'.format(i, 2 ** (l + 1), v, t))


def systematic_encoder_decoder_chain_test():
    print('systematic encoder decoder chain test')
    block_size = int(2 ** 8)
    info_bit_size = block_size // 2
    ntests = 100
    frozenbitposition = cc.get_frozen_bit_indices_from_z_parameters(cc.bhattacharyya_bounds(0.0, block_size), block_size - info_bit_size)
    encoder = PolarEncoder(block_size, info_bit_size, frozenbitposition)
    decoder = PolarDecoder(block_size, info_bit_size, frozenbitposition)
    for i in range(ntests):
        bits = np.random.randint(2, size=info_bit_size)
        y = encoder.encode_systematic(bits)
        u_hat = decoder.decode_systematic(y)
        assert (bits == u_hat).all()


def main():
    n = 8
    m = 2 ** n
    k = m // 2
    n_frozen = n - k
    # n = 16
    # k = 8
    # frozenbits = np.zeros(n - k)
    # frozenbitposition8 = np.array((0, 1, 2, 4), dtype=int)
    # frozenbitposition = np.array((0, 1, 2, 3, 4, 5, 8, 9), dtype=int)
    # print frozenbitposition

    # test_enc_dec_chain()
    # test_1024_rate_1_code()
    # channel_analysis()

    # frozen_indices = cc.get_bec_frozen_indices(m, n_frozen, 0.11)
    # frozen_mask = cc.get_frozen_bit_mask(frozen_indices, m)
    # find_decoder_subframes(frozen_mask)

    systematic_encoder_decoder_chain_test()


if __name__ == '__main__':
    main()
