#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# turn this folder into a Python module

from . import channel_construction as cc
from .channel_construction_bec import bhattacharyya_bounds
from .helper_functions import is_power_of_two


CHANNEL_TYPE_AWGN = 'AWGN'
CHANNEL_TYPE_BEC = 'BEC'

def get_z_params(is_prototype, channel, block_size, design_snr, mu):
    print('POLAR code channel construction called with parameters channel={0}, blocksize={1}, design SNR={2}, mu={3}'.format(channel, block_size, design_snr, mu))
    if not (channel == 'AWGN' or channel == 'BEC'):
        raise ValueError("channel is {0}, but only BEC and AWGN are supported!".format(channel))
    if not is_power_of_two(block_size):
        raise ValueError("block size={0} is not a power of 2!".format(block_size))
    if design_snr < -1.5917:
        raise ValueError("design SNR={0} < -1.5917. MUST be greater!".format(design_snr))
    if not mu > 0:
        raise ValueError("mu={0} < 1. MUST be > 1!".format(mu))
    if not is_prototype and channel == 'AWGN':
        z_params = cc.load_z_parameters(block_size, design_snr, mu)
        print('Read Z-parameter file: {0}'.format(cc.default_dir() + cc.generate_filename(block_size, design_snr, mu)))
        return z_params
    return bhattacharyya_bounds(design_snr, block_size)


def load_frozen_bits_info(is_prototype, channel, block_size, num_info_bits, design_snr, mu):
    num_frozen_bits = block_size - num_info_bits
    if not mu > 0:
        mu = 2
    z_params = get_z_params(is_prototype, channel, block_size, design_snr, mu)
    data_set = {
        'positions': cc.get_frozen_bit_indices_from_z_parameters(z_params, num_frozen_bits),
        'values': [0, ] * num_frozen_bits,
        'block_size': block_size,
        'num_info_bits': num_info_bits,
        'num_frozenbits': num_frozen_bits,
        'design_snr': design_snr,
        'channel': channel,
        'mu': mu,
        }
    return data_set
