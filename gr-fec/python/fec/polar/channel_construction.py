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


from channel_construction_bec import calculate_bec_channel_capacities
from channel_construction_bec import design_snr_to_bec_eta
from channel_construction_bec import bhattacharyya_bounds
from helper_functions import *
try:
    from channel_construction_awgn import tal_vardy_tpm_algorithm
except ImportError:
    print("SciPy missing. Overwrite Tal-Vardy algorithm with BEC approximation")
    def tal_vardy_tpm_algorithm(block_size, design_snr, mu):
        return bhattacharyya_bounds(design_snr, block_size)



Z_PARAM_FIRST_HEADER_LINE = "Bhattacharyya parameters (Z-parameters) for a polar code"


def get_frozen_bit_indices_from_capacities(chan_caps, nfrozen):
    indexes = np.array([], dtype=int)
    while indexes.size < nfrozen:
        index = np.argmin(chan_caps).astype(int)
        indexes = np.append(indexes, index)
        chan_caps[index] = 2.0  # make absolutely sure value is out of range!
    return np.sort(indexes)


def get_frozen_bit_indices_from_z_parameters(z_params, nfrozen):
    indexes = np.array([], dtype=int)
    while indexes.size < nfrozen:
        index = np.argmax(z_params).astype(int)
        indexes = np.append(indexes, index)
        z_params[index] = -1.0
    return np.sort(indexes)


def get_bec_frozen_indices(nblock, kfrozen, eta):
    bec_caps = calculate_bec_channel_capacities(eta, nblock)
    positions = get_frozen_bit_indices_from_capacities(bec_caps, kfrozen)
    return positions


def get_frozen_bit_mask(frozen_indices, block_size):
    frozen_mask = np.zeros(block_size, dtype=int)
    frozen_mask[frozen_indices] = 1
    return frozen_mask


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


def save_z_parameters(z_params, block_size, design_snr, mu, alt_construction_method='Tal-Vardy algorithm'):
    path = default_dir()
    filename = generate_filename(block_size, design_snr, mu)
    header = Z_PARAM_FIRST_HEADER_LINE + "\n"
    header += "Channel construction method: " + alt_construction_method + "\n"
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


def main():
    np.set_printoptions(precision=3, linewidth=150)
    print('channel construction Bhattacharyya bounds by Arikan')
    n = 10
    m = 2 ** n
    k = m // 2
    design_snr = 0.0
    mu = 32

    z_params = load_z_parameters(m, design_snr, mu)
    z_bounds = bhattacharyya_bounds(design_snr, m)
    print(z_params[-10:])

    if 0:
        import matplotlib.pyplot as plt
        plt.plot(z_params)
        plt.plot(z_bounds)
        plt.show()


if __name__ == '__main__':
    main()
