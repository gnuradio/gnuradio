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

import numpy, pylab, sys
from gnuradio import digital
from soft_dec_lut_gen import soft_dec_table, calc_soft_dec_from_table, calc_soft_dec
from psk_constellations import psk_4_0, psk_4_1, psk_4_2, psk_4_3, psk_4_4, psk_4_5, psk_4_6, psk_4_7, sd_psk_4_0, sd_psk_4_1, sd_psk_4_2, sd_psk_4_3, sd_psk_4_4, sd_psk_4_5, sd_psk_4_6, sd_psk_4_7
from qam_constellations import qam_16_0, sd_qam_16_0

def test_qpsk(i, sample, prec):
    qpsk_const_list = [psk_4_0, psk_4_1, psk_4_2, psk_4_3,
                       psk_4_4, psk_4_5, psk_4_6, psk_4_7]
    qpsk_lut_gen_list = [sd_psk_4_0, sd_psk_4_1, sd_psk_4_2, sd_psk_4_3,
                         sd_psk_4_4, sd_psk_4_5, sd_psk_4_6, sd_psk_4_7]

    constel, code = qpsk_const_list[i]()
    qpsk_lut_gen = qpsk_lut_gen_list[i]

    rot_sym = 1
    side = 2
    width = 2
    c = digital.constellation_rect(constel, code, rot_sym,
                                   side, side, width, width)

    # Get max energy/symbol in constellation
    constel = c.points()
    Es = max([numpy.sqrt(constel_i.real**2 + constel_i.imag**2) for constel_i in constel])

    #table = soft_dec_table_generator(qpsk_lut_gen, prec, Es)
    table = soft_dec_table(constel, code, prec)

    c.gen_soft_dec_lut(prec)
    #c.set_soft_dec_lut(table, prec)

    y_python_gen_calc = qpsk_lut_gen(sample, Es)
    y_python_table = calc_soft_dec_from_table(sample, table, prec, Es)
    y_python_raw_calc = calc_soft_dec(sample, constel, code)
    y_cpp_table = c.soft_decision_maker(sample)
    y_cpp_raw_calc = c.calc_soft_dec(sample)

    return (y_python_gen_calc, y_python_table, y_python_raw_calc,
            y_cpp_table, y_cpp_raw_calc, constel, code, c)

def test_qam16(i, sample, prec):
    sample = sample/1
    qam_const_list = [qam_16_0, ]
    qam_lut_gen_list = [sd_qam_16_0, ]

    constel, code = qam_const_list[i]()
    qam_lut_gen = qam_lut_gen_list[i]

    rot_sym = 4
    side = 2
    width = 2
    c = digital.constellation_rect(constel, code, rot_sym,
                                   side, side, width, width)

    # Get max energy/symbol in constellation
    constel = c.points()
    Es = max([abs(constel_i) for constel_i in constel])

    #table = soft_dec_table_generator(qam_lut_gen, prec, Es)
    table = soft_dec_table(constel, code, prec, 1)

    #c.gen_soft_dec_lut(prec)
    c.set_soft_dec_lut(table, prec)

    y_python_gen_calc = qam_lut_gen(sample, Es)
    y_python_table = calc_soft_dec_from_table(sample, table, prec, Es)
    y_python_raw_calc = calc_soft_dec(sample, constel, code, 1)
    y_cpp_table = c.soft_decision_maker(sample)
    y_cpp_raw_calc = c.calc_soft_dec(sample)

    return (y_python_gen_calc, y_python_table, y_python_raw_calc,
            y_cpp_table, y_cpp_raw_calc, constel, code, c)

if __name__ == "__main__":

    index = 0
    prec = 8

    x_re = 2*numpy.random.random()-1
    x_im = 2*numpy.random.random()-1
    x = x_re + x_im*1j
    #x = -1 + -0.j

    if 1:
        y_python_gen_calc, y_python_table, y_python_raw_calc, \
            y_cpp_table, y_cpp_raw_calc, constel, code, c \
            = test_qpsk(index, x, prec)
    else:
        y_python_gen_calc, y_python_table, y_python_raw_calc, \
            y_cpp_table, y_cpp_raw_calc, constel, code, c \
            = test_qam16(index, x, prec)

    k = numpy.log2(len(constel))

    print "Sample: ", x
    print "Python Generator Calculated: ", (y_python_gen_calc)
    print "Python Generator Table:      ", (y_python_table)
    print "Python Raw calc:             ", (y_python_raw_calc)
    print "C++ Table calc:              ", (y_cpp_table)
    print "C++ Raw calc:                ", (y_cpp_raw_calc)

    fig = pylab.figure(1)
    sp1 = fig.add_subplot(1,1,1)
    sp1.plot([c.real for c in constel],
             [c.imag for c in constel], 'bo')
    sp1.plot(x.real, x.imag, 'ro')
    sp1.set_xlim([-1.5, 1.5])
    sp1.set_ylim([-1.5, 1.5])
    fill = int(numpy.log2(len(constel)))
    for i,c in enumerate(constel):
        sp1.text(1.2*c.real, 1.2*c.imag, bin(code[i])[2:].zfill(fill),
                 ha='center', va='center', size=18)
    pylab.show()
