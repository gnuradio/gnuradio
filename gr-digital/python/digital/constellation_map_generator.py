#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

def constellation_map_generator(basis_cpoints, basis_symbols, k, pi):
    '''
    Uses the a basis constellation provided (e.g., from
    psk_constellation.psk_4()) and the the k and permutation index
    (pi) to generate a new Gray-coded symbol map to the constellation
    points provided in the basis.

    The basis_cpoints are the constellation points of the basis
    constellation, and basis_symbols are the symbols that correspond
    to the constellation points.

    The selection of k and pi will provide an automorphism the
    hyperoctahedral group of the basis constellation.

    This function returns a tuple of (constellation_points,
    symbol_map). The constellation_points is a list of the
    constellation points in complex space and the symbol_map is a list
    of the log2(M)-bit symbols for the constellation points (i.e.,
    symbol_map[i] are the bits associated with
    constellation_points[i]).
    '''
    #const_points, s = basis()
    const_points = basis_cpoints
    s = basis_symbols
    symbols = list()
    for s_i in s:
        tmp = 0
        for i,p in enumerate(pi):
            bit = (s_i >> i) & 0x1
            tmp |= bit << p
        symbols.append(tmp ^ k)
    return (const_points, symbols)
