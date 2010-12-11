#
# Copyright 2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
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

"""
QAM modulation and demodulation.
"""

from math import pi, sqrt, log
from itertools import islice

from gnuradio import gr, gru, modulation_utils2
from gnuradio.blks2impl.generic_mod_demod import generic_mod, generic_demod


# default values (used in __init__ and add_options)
_def_constellation_points = 16
_def_samples_per_symbol = 2
_def_excess_bw = 0.35
_def_verbose = False
_def_log = False

# Frequency correction
_def_freq_alpha = 0.010
# Symbol timing recovery 
_def_timing_alpha = 0.100
_def_timing_beta = 0.010
_def_timing_max_dev = 1.5
# Fine frequency / Phase correction
_def_phase_alpha = 0.1

def is_power_of_four(x):
    v = log(x)/log(4)
    return int(v) == v

def get_bit(x, n):
    """ Get the n'th bit of integer x (from little end)."""
    return (x&(0x01 << n)) >> n

def get_bits(x, n, k):
    """ Get the k bits of integer x starting at bit n(from little end)."""
    # Remove the n smallest bits
    v = x >> n 
    # Remove all bits bigger than n+k-1
    return v % pow(2, k)

def gray_codes():
    """ Generates gray codes."""
    gcs = [0, 1]
    yield 0
    yield 1
    # The last power of two passed through.
    lp2 = 2
    # The next power of two that will be passed through.
    np2 = 4
    i = 2
    while True:
        if i == lp2:
            # if i is a power of two then gray number is of form 1100000...
            result = i + i/2
        else:
            # if not we take advantage of the symmetry of all but the last bit
            # around a power of two.
            result = gcs[2*lp2-1-i] + lp2
        gcs.append(result)
        yield result
        i += 1
        if i == np2:
            lp2 = i
            np2 = i*2

def make_constellation(m):
    """
    Create a constellation with m possible symbols where m must be
    a power of 4.

    Points are laid out in a square grid.

    """
    sqrtm = pow(m, 0.5)
    if (not isinstance(m, int) or m < 4 or not is_power_of_four(m)):
        raise ValueError("m must be a power of 4 integer.")
    # Each symbol holds k bits.
    k = int(log(m) / log(2.0))
    # First create a constellation for one quadrant containing m/4 points.
    # The quadrant has 'side' points along each side of a quadrant.
    side = int(sqrtm/2)
    # Number rows and columns using gray codes.
    gcs = list(islice(gray_codes(), side))
    # Get inverse gray codes.
    i_gcs = dict([(v, key) for key, v in enumerate(gcs)])
    # The distance between points is found.
    step = 1/(side-0.5)

    gc_to_x = [(i_gcs[gc]+0.5)*step for gc in range(0, side)]

    # Takes the (x, y) location of the point with the quadrant along
    # with the quadrant number. (x, y) are integers referring to which
    # point within the quadrant it is.
    # A complex number representing this location of this point is returned.
    def get_c(gc_x, gc_y, quad):
        if quad == 0:
            return complex(gc_to_x[gc_x], gc_to_x[gc_y])
        if quad == 1:
            return complex(-gc_to_x[gc_y], gc_to_x[gc_x])
        if quad == 2:
            return complex(-gc_to_x[gc_x], -gc_to_x[gc_y])
        if quad == 3:
            return complex(gc_to_x[gc_y], -gc_to_x[gc_x])
        raise StandardError("Impossible!")

    # First two bits determine quadrant.
    # Next (k-2)/2 bits determine x position.
    # Following (k-2)/2 bits determine y position.
    # How x and y relate to real and imag depends on quadrant (see get_c function).
    const_map = []
    for i in range(m):
        y = get_bits(i, 0, (k-2)/2)
        x = get_bits(i, (k-2)/2, (k-2)/2)
        quad = get_bits(i, k-2, 2)
        const_map.append(get_c(x, y, quad))

    return const_map
        

# /////////////////////////////////////////////////////////////////////////////
#                           QAM modulator
# /////////////////////////////////////////////////////////////////////////////

class qam_mod(generic_mod):

    def __init__(self, constellation_points=_def_constellation_points,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 verbose=_def_verbose,
                 log=_def_log):

        """
	Hierarchical block for RRC-filtered QAM modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        @param m: Number of constellation points.  Must be a power of four.
        @type m: integer
	@param samples_per_symbol: samples per baud >= 2
	@type samples_per_symbol: integer
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Log modulation data to files?
        @type log: bool
	"""

        if not isinstance(constellation_points, int) or not is_power_of_four(constellation_points):
            raise ValueError("number of constellation points must be a power of four.")

        points = make_constellation(constellation_points)
        constellation = gr.gr_constellation(points)

        super(qam_mod, self).__init__(constellation, samples_per_symbol,
                                      excess_bw, verbose, log)

    def add_options(parser):
        """
        Adds QAM modulation-specific options to the standard parser
        """
        parser.add_option("-p", "--constellation-points", type="int", default=_def_constellation_points,
                          help="set the number of constellation points (must be a power of 4) [default=%default]")
        generic_mod.add_options(parser)
    add_options=staticmethod(add_options)

# /////////////////////////////////////////////////////////////////////////////
#                           QAM demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class qam_demod(generic_demod):

    def __init__(self, constellation_points=_def_constellation_points,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 freq_alpha=_def_freq_alpha,
                 timing_alpha=_def_timing_alpha,
                 timing_max_dev=_def_timing_max_dev,
                 phase_alpha=_def_phase_alpha,
                 verbose=_def_verbose,
                 log=_def_log):

        """
	Hierarchical block for RRC-filtered QAM modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

        @param m: Number of constellation points.  Must be a power of four.
        @type m: integer
	@param samples_per_symbol: samples per symbol >= 2
	@type samples_per_symbol: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param freq_alpha: loop filter gain for frequency recovery
        @type freq_alpha: float
        @param timing_alpha: loop alpha gain for timing recovery
        @type timing_alpha: float
        @param timing_max_dev: timing loop maximum rate deviations
        @type timing_max_dev: float
        @param phase_alpha: loop filter gain in phase loop
        @type phase_alphas: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param debug: Print modualtion data to files?
        @type debug: bool
        """

        points = make_constellation(constellation_points)
        constellation = gr.gr_constellation(points)

        super(qam_demod, self).__init__(constellation, samples_per_symbol,
                                        excess_bw, freq_alpha, timing_alpha,
                                        timing_max_dev, phase_alpha, verbose,
                                        log)

    def add_options(parser):
        """
        Adds QAM demodulation-specific options to the standard parser
        """
        parser.add_option("", "--constellation-points", type="int", default=_def_constellation_points,
                          help="set the number of constellation points (must be a power of 4) [default=%default]")
        generic_demod.add_options(parser)
    add_options=staticmethod(add_options)

#
# Add these to the mod/demod registry
#
modulation_utils2.add_type_1_mod('qam', qam_mod)
modulation_utils2.add_type_1_demod('qam', qam_demod)
