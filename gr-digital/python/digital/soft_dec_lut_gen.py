#!/usr/bin/env python
#
# Copyright 2013-2014 Free Software Foundation, Inc.
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

import numpy

def soft_dec_table_generator(soft_dec_gen, prec, Es=1):
    '''
    | Builds a LUT that is a list of tuples. The tuple represents the
    | soft decisions for the constellation/bit mapping at any given
    | point in the complex space, (x,y).
    |
    | The table is built to a precision specified by the 'prec'
    | argument. There are (2x2)^prec samples in the sample space, so we
    | get the precision of 2^prec samples in both the real and imaginary
    | axes.
    |
    | The space is represented where index 0 is the bottom left corner
    | and the maximum index is the upper left. The table index for a
    | surface space with 4 bits of precision looks like the following:
    |
    | 240 241 242 243 244 245 246 247 | 248 249 250 251 252 253 254 255
    | 224 225 226 227 228 229 230 231 | 232 233 234 235 236 237 238 239
    | 208 209 210 211 212 213 214 215 | 216 217 218 219 220 221 222 223
    | 192 193 194 195 196 197 198 199 | 200 201 202 203 204 205 206 207
    | 176 177 178 179 180 181 182 183 | 184 185 186 187 188 189 190 191
    | 160 161 162 163 164 165 166 167 | 168 169 170 171 172 173 174 175
    | 144 145 146 147 148 149 150 151 | 152 153 154 155 156 157 158 159
    | 128 129 130 131 132 133 134 135 | 136 137 138 139 140 141 142 143
    | -----------------------------------------------------------------
    | 112 113 114 115 116 117 118 119 | 120 121 122 123 124 125 126 127
    |  96  97  98  99 100 101 102 103 | 104 105 106 107 108 109 110 111
    |  80  81  82  83  84  85  86  87 |  88  89  90  91  92  93  94  95
    |  64  65  66  67  68  69  70  71 |  72  73  74  75  76  77  78  79
    |  48  49  50  51  52  53  54  55 |  56  57  58  59  60  61  62  63
    |  32  33  34  35  36  37  38  39 |  40  41  42  43  44  45  46  47
    |  16  17  18  19  20  21  22  23 |  24  25  26  27  28  29  30  31
    |   0   1   2   3   4   5   6   7 |   8   9  10  11  12  13  14  15
    |
    | We then calculate coordinates from -1 to 1 with 2^prec points for
    | both the x and y axes. We then sample starting at (-1, -1) and
    | move left to right on the x-axis and then move up a row on the
    | y-axis. For every point in this sampled space, we calculate the
    | soft decisions for the given constellation/mapping. This is done
    | by passing in the function 'soft_dec_gen' as an argument to this
    | function. This takes in the x/y coordinates and outputs the soft
    | decisions. These soft decisions are stored into the list at the
    | index from the above table as a tuple.
    |
    | The function 'calc_from_table' takes in a point and reverses this
    | operation. It converts the point from the coordinates (-1,-1) to
    | (1,1) into an index value in the table and returns the tuple of
    | soft decisions at that index.
    |
    | Es is the maximum energy per symbol. This is passed to the
    | function to provide the bounds when calling the generator function
    | since they don't know how the constellation was normalized. Using
    | the (maximum) energy per symbol for constellation allows us to
    | provide any scaling of the constellation (normalized to sum to 1,
    | normalized so the outside points sit on +/-1, etc.) but still
    | calculate the soft decisions as we would given the full
    | constellation.

    '''

    npts = 2.0**prec
    maxd = Es*numpy.sqrt(2.0)/2.0
    yrng = numpy.linspace(-maxd, maxd, npts)
    xrng = numpy.linspace(-maxd, maxd, npts)

    table = []
    for y in yrng:
        for x in xrng:
            pt = complex(x, y)
            decs = soft_dec_gen(pt, Es)
            table.append(decs)
    return table

def soft_dec_table(constel, symbols, prec, npwr=1):
    '''
    Similar in nature to soft_dec_table_generator above. Instead, this
    takes in the constellation and symbol points along with the noise
    power estimate and uses calc_soft_dec (below) to generate the
    LUT.

    Instead of assuming that the constellation is normalied (e.g., all
    points are between -1 and 1), this function calculates the min/max
    of both the real and imaginary axes and uses those when
    constructing the LUT. So when using this version of the LUT, the
    samples and the constellations must be working on the same
    magnitudes.

    Because this uses the calc_soft_dec function, it can be quite
    a bit more expensive to generate the LUT, though it should be
    one-time work.
    '''

    re_min = min(numpy.array(constel).real)
    im_min = min(numpy.array(constel).imag)
    re_max = max(numpy.array(constel).real)
    im_max = max(numpy.array(constel).imag)

    npts = 2.0**prec
    yrng = numpy.linspace(im_min, im_max, npts)
    xrng = numpy.linspace(re_min, re_max, npts)

    table = []
    for y in yrng:
        for x in xrng:
            pt = complex(x, y)
            decs = calc_soft_dec(pt, constel, symbols, npwr)
            table.append(decs)
    return table

def calc_soft_dec_from_table(sample, table, prec, Es=1.0):
    '''
    Takes in a complex sample and converts it from the coordinates
    (-1,-1) to (1,1) into an index value. The index value points to a
    location in the provided LUT 'table' and returns the soft
    decisions tuple at that index.

    sample: the complex sample to calculate the soft decisions
    from.

    table: the LUT.

    prec: the precision used when generating the LUT.

    Es: the energy per symbol. This is passed to the function to
    provide the bounds when calling the generator function since they
    don't know how the constellation was normalized. Using the
    (maximum) energy per symbol for constellation allows us to provide
    any scaling of the constellation (normalized to sum to 1,
    normalized so the outside points sit on +/-1, etc.) but still
    calculate the soft decisions as we would given the full
    constellation.
    '''
    lut_scale = 2.0**prec
    maxd = Es*numpy.sqrt(2.0)/2.0
    scale = (lut_scale) / (2.0*maxd)

    alpha = 0.99 # to keep index within bounds
    xre = sample.real
    xim = sample.imag
    xre = ((maxd + min(alpha*maxd, max(-alpha*maxd, xre))) * scale)
    xim = ((maxd + min(alpha*maxd, max(-alpha*maxd, xim))) * scale)
    index = int(xre) + lut_scale*int(xim)

    max_index = lut_scale**2

    while(index >= max_index):
        index -= lut_scale;
    while(index < 0):
        index += lut_scale;

    return table[int(index)]

def calc_soft_dec(sample, constel, symbols, npwr=1):
    '''
    This function takes in any consteallation and symbol symbol set
    (where symbols[i] is the set of bits at constellation point
    constel[i] and an estimate of the noise power and produces the
    soft decisions for the given sample.

    If known, the noise power of the received sample may be passed in
    to this function as npwr.

    This is an incredibly costly algorthm because it must calculate
    the Euclidean distance between the sample and all points in the
    constellation to build up its probability
    calculations. Conversely, it should work for any given
    constellation/symbol map.

    The function returns a vector of k soft decisions. Decisions less
    than 0 are more likely to indicate a '0' bit and decisions greater
    than 0 are more likely to indicate a '1' bit.
    '''

    M = len(constel)
    k = int(numpy.log2(M))
    tmp = 2*k*[0,]
    s = k*[0,]

    for i in range(M):
        # Calculate the distance between the sample and the current
        # constellation point.
        dist = abs(sample - constel[i])

        # Calculate the probability factor from the distance and the
        # scaled noise power.
        d = numpy.exp(-dist/npwr)

        for j in range(k):
            # Get the bit at the jth index
            mask = 1<<j
            bit = (symbols[i] & mask) >> j

            # If the bit is a 0, add to the probability of a zero
            if(bit == 0):
                tmp[2*j+0] += d
            # else, add to the probability of a one
            else:
                tmp[2*j+1] += d

    # Calculate the log-likelihood ratio for all bits based on the
    # probability of ones (tmp[2*i+1]) over the probability of a zero
    # (tmp[2*i+0]).
    for i in range(k):
        s[k-1-i] = (numpy.log(tmp[2*i+1]) - numpy.log(tmp[2*i+0]))

    return s


def show_table(table):
    prec = int(numpy.sqrt(len(table)))
    nbits = len(table[0])
    pp = ""
    subi = 1
    subj = 0
    for i in reversed(xrange(prec+1)):
        if(i == prec//2):
            pp += "-----" + prec*((nbits*8)+3)*"-" + "\n"
            subi = 0
            continue
        for j in xrange(prec+1):
            if(j == prec//2):
                pp += "| "
                subj = 1
            else:
                item = table[prec*(i-subi) + (j-subj)]
                pp += "( "
                for t in xrange(nbits-1, -1, -1):
                    pp += "{0: .4f} ".format(item[t])
                pp += ") "
        pp += "\n"
        subj = 0
    print pp
