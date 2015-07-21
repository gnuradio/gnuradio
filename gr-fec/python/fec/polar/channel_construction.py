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

'''
[0] Erdal Arikan: 'Channel Polarization: A Method for Constructing Capacity-Achieving Codes for Symmetric Binary-Input Memoryless Channels', 2009
foundational paper for polar codes.
'''


import numpy as np
from channel_construction_bec import calculate_bec_channel_capacities
from channel_construction_bec import design_snr_to_bec_eta
from channel_construction_bsc import tal_vardy_tpm_algorithm
from helper_functions import *
import matplotlib.pyplot as plt


Z_PARAM_FIRST_HEADER_LINE = "Bhattacharyya parameters (Z-parameters) for a polar code"


def get_frozen_bit_indices_from_capacities(chan_caps, nfrozen):
    indexes = np.array([], dtype=int)
    while indexes.size < nfrozen:
        index = np.argmin(chan_caps)
        indexes = np.append(indexes, index)
        chan_caps[index] = 2.0  # make absolutely sure value is out of range!
    return np.sort(indexes)


def get_frozen_bit_indices_from_z_parameters(z_params, nfrozen):
    indexes = np.array([], dtype=int)
    while indexes.size < nfrozen:
        index = np.argmax(z_params)
        indexes = np.append(indexes, index)
        z_params[index] = 0.0
    return np.sort(indexes)


def get_bec_frozen_indices(nblock, kfrozen, eta):
    bec_caps = calculate_bec_channel_capacities(eta, nblock)
    positions = get_frozen_bit_indices_from_capacities(bec_caps, kfrozen)
    return positions


def frozen_bit_positions(block_size, info_size, design_snr=0.0):
    if not design_snr > -1.5917:
        print('bad value for design_nsr, must be > -1.5917! default=0.0')
        design_snr = 0.0
    eta = design_snr_to_bec_eta(design_snr)
    return get_bec_frozen_indices(block_size, block_size - info_size, eta)


def generate_filename(block_size, design_snr, mu):
    filename = "polar_code_z_parameters_N" + str(int(block_size))
    filename += "_SNR" + str(float(design_snr)) + "_MU" + str(int(mu)) + ".polar"
    return filename


def default_dir():
    dir_def = "~/.gnuradio/polar/"
    import os
    path = os.path.expanduser(dir_def)

    try:
        os.makedirs(path)
    except OSError:
        if not os.path.isdir(path):
            raise
    return path


def save_z_parameters(z_params, block_size, design_snr, mu):
    path = default_dir()
    filename = generate_filename(block_size, design_snr, mu)
    header = Z_PARAM_FIRST_HEADER_LINE + "\n"
    header += "Channel construction method: Tal-Vardy algorithm\n"
    header += "Parameters:\n"
    header += "block_size=" + str(block_size) + "\n"
    header += "design_snr=" + str(design_snr) + "\n"
    header += "mu=" + str(mu)
    np.savetxt(path + filename, z_params, header=header)


def load_z_parameters(block_size, design_snr, mu):
    path = default_dir()
    filename = generate_filename(block_size, design_snr, mu)
    full_file = path + filename
    import os
    if not os.path.isfile(full_file):
        z_params = tal_vardy_tpm_algorithm(block_size, design_snr, mu)
        save_z_parameters(z_params, block_size, design_snr, mu)
    z_params = np.loadtxt(full_file)
    return z_params


def prepare_merger(frozen_mask):
    mask = []
    for e in frozen_mask:
        mask.append([e, ])
    return np.array(mask, dtype=int)


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
    frame_size = 2 ** stages

    lock_mask = np.zeros(frame_size, dtype=int)
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
        for p in range(0, (frame_size // stage_size) - 1, 2):
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



def main():
    np.set_printoptions(precision=3, linewidth=150)
    print 'channel construction Bhattacharyya bounds by Arikan'
    n = 10
    m = 2 ** n
    k = m // 2
    design_snr = -1.59
    mu = 32

    z_params = load_z_parameters(m, design_snr, mu)
    # plt.plot(z_params)
    # plt.show()
    frozen_indices = get_frozen_bit_indices_from_z_parameters(z_params, k)

    frozen_mask = np.zeros(m, dtype=int)
    frozen_mask[frozen_indices] = 1
    # frozen_mask = np.reshape(frozen_mask, (-1, 32))
    # for p in frozen_mask:
    #     print(p)
    #     if np.all(p == 1):
    #         print("zero rate")
    #     elif np.all(p == 0):
    #         print("ONE rate")
    #     elif p[0] == 1 and np.all(p[1:] == 0):
    #         print("SPC code")
    #     elif np.all(p[0:-1] == 1) and p[-1] == 0:
    #         print("REPETITION code")

    find_decoder_subframes(frozen_mask)




if __name__ == '__main__':
    main()


