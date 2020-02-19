#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import unicode_literals
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
