#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy
from matplotlib import pyplot
from gnuradio import digital
from gnuradio.digital.soft_dec_lut_gen import (
    calc_soft_dec_from_table, calc_soft_dec, soft_dec_table_generator
)
from gnuradio.digital.psk_constellations import (
    psk_4_0, psk_4_1, psk_4_2, psk_4_3, psk_4_4, psk_4_5, psk_4_6, psk_4_7,
    sd_psk_4_0, sd_psk_4_1, sd_psk_4_2, sd_psk_4_3, sd_psk_4_4, sd_psk_4_5,
    sd_psk_4_6, sd_psk_4_7
)
from gnuradio.digital.qam_constellations import qam_16_0, sd_qam_16_0


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
    Es = max(abs(pt) ** 2 for pt in constel)
    max_amp = numpy.sqrt(Es)
    table = soft_dec_table_generator(qpsk_lut_gen, prec, Es)
    c.set_soft_dec_lut(table, prec)

    py_soft_dec = qpsk_lut_gen(sample, Es)
    py_lut = calc_soft_dec_from_table(sample, table, prec, max_amp)
    py_llr = calc_soft_dec(sample, constel, code)
    cpp_lut = c.soft_decision_maker(sample)
    cpp_llr = c.calc_soft_dec(sample)

    soft_decisions = {"python_soft_decision": py_soft_dec,
                      "python_lut": py_lut,
                      "python_llr": py_llr,
                      "cpp_lut": cpp_lut,
                      "cpp_llr": cpp_llr}

    return soft_decisions, constel, code, c


def test_qam16(i, sample, prec):
    sample = sample / 1
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
    Es = max(abs(pt) ** 2 for pt in constel)
    max_amp = numpy.sqrt(Es)
    table = soft_dec_table_generator(qam_lut_gen, prec, Es)
    c.set_soft_dec_lut(table, prec)

    py_soft_dec = qam_lut_gen(sample, Es)
    py_lut = calc_soft_dec_from_table(sample, table, prec, max_amp)
    py_llr = calc_soft_dec(sample, constel, code, 1)
    cpp_lut = c.soft_decision_maker(sample)
    cpp_llr = c.calc_soft_dec(sample)
    soft_decisions = {"python_soft_decision": py_soft_dec,
                      "python_lut": py_lut,
                      "python_llr": py_llr,
                      "cpp_lut": cpp_lut,
                      "cpp_llr": cpp_llr}

    return soft_decisions, constel, code, c


def show_test_results(soft_decs, constel, code, c, title):
    print("\n" + title)
    print("Sample: ", x)
    print("Python soft decision: ", soft_decs["python_soft_decision"])
    print("Python lookup table:  ", soft_decs["python_lut"])
    print("C++ lookup table:     ", soft_decs["cpp_lut"])
    print("Python raw LLR calc:  ", soft_decs["python_llr"])
    print("C++ raw LLR calc:     ", soft_decs["cpp_llr"])

    fig = pyplot.figure(1)
    sp1 = fig.add_subplot(1, 1, 1)
    sp1.plot([c.real for c in constel],
             [c.imag for c in constel], 'bo')
    sp1.plot(x.real, x.imag, 'ro')
    sp1.set_xlim([-1.5, 1.5])
    sp1.set_ylim([-1.5, 1.5])
    fill = int(numpy.log2(len(constel)))
    for i, c in enumerate(constel):
        sp1.text(1.2 * c.real, 1.2 * c.imag, bin(code[i])[2:].zfill(fill),
                 ha='center', va='center', size=18)
    pyplot.title(title)
    pyplot.show()


if __name__ == "__main__":
    prec = 8
    x_re = 2 * numpy.random.random() - 1
    x_im = 2 * numpy.random.random() - 1
    x = x_re + x_im * 1j

    for index in range(8):
        soft_decs, constel, code, c = test_qpsk(index, x, prec)
        show_test_results(soft_decs, constel, code, c,
                          title="QPSK " + str(index))

    for index in range(1):
        soft_decs, constel, code, c = test_qam16(index, x, prec)
        show_test_results(soft_decs, constel, code, c,
                          title="QAM " + str(index))
