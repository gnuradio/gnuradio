#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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

def bitreverse(mint):
    res = 0;
    while mint != 0:
        res = res << 1;
        res += mint & 1;
        mint = mint >> 1;
    return res;

const_lut = [2];
specinvert_lut = [[0, 2, 1, 3]];

def bitflip(mint, bitflip_lut, index, csize):
    res = 0;
    cnt = 0;
    mask = (1 << const_lut[index]) - 1;
    while (cnt < csize):
        res += (bitflip_lut[(mint >> cnt) & (mask)]) << cnt;
        cnt += const_lut[index];
    return res;


def read_bitlist(bitlist):
    res = 0;
    for i in range(len(bitlist)):
        if int(bitlist[i]) == 1:
            res += 1 << (len(bitlist) - i - 1);
    return res;


def read_big_bitlist(bitlist):
    ret = []
    for j in range(0, len(bitlist)/64):
        res = 0;
        for i in range(0, 64):
            if int(bitlist[j*64+i]) == 1:
                res += 1 << (64 - i - 1);
        ret.append(res);
    res = 0;
    j = 0;
    for i in range(len(bitlist)%64):
        if int(bitlist[len(ret)*64+i]) == 1:
            res += 1 << (64 - j - 1);
        j += 1;
    ret.append(res);
    return ret;

def generate_symmetries(symlist):
    retlist = []
    if len(symlist) == 1:
        for i in range(len(symlist[0])):
            retlist.append(symlist[0][i:] + symlist[0][0:i]);
        invlist = symlist[0];
        for i in range(1, len(symlist[0])/2):
            invlist[i] = symlist[0][i + len(symlist[0])/2];
            invlist[i + len(symlist[0])/2] = symlist[0][i];
        for i in range(len(symlist[0])):
            retlist.append(symlist[0][i:] + symlist[0][0:i]);
        return retlist;
