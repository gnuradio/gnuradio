#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest

import random
import numpy
from gnuradio import digital, blocks, channels


class qa_linear_equalizer(gr_unittest.TestCase):

    def unpack_values(self, values_in, bits_per_value, bits_per_symbol):
        # verify that 8 is divisible by bits_per_symbol
        m = bits_per_value / bits_per_symbol
        # print(m)
        mask = 2**(bits_per_symbol) - 1

        if bits_per_value != m * bits_per_symbol:
            print(
                "error - bits per symbols must fit nicely into bits_per_value bit values")
            return []

        num_values = len(values_in)
        num_symbols = int(num_values * (m))

        cur_byte = 0
        cur_bit = 0
        out = []
        for i in range(num_symbols):
            s = (
                values_in[cur_byte] >> (
                    bits_per_value -
                    bits_per_symbol -
                    cur_bit)) & mask
            out.append(s)
            cur_bit += bits_per_symbol

            if cur_bit >= bits_per_value:
                cur_bit = 0
                cur_byte += 1

        return out

    def map_symbols_to_constellation(self, symbols, cons):
        l = list(map(lambda x: cons.points()[x], symbols))
        return l

    def setUp(self):
        random.seed(987654)
        self.tb = gr.top_block()
        self.num_data = num_data = 10000

        self.sps = sps = 4
        self.eb = eb = 0.35
        self.preamble = preamble = [
            0x27,
            0x2F,
            0x18,
            0x5D,
            0x5B,
            0x2A,
            0x3F,
            0x71,
            0x63,
            0x3C,
            0x17,
            0x0C,
            0x0A,
            0x41,
            0xD6,
            0x1F,
            0x4C,
            0x23,
            0x65,
            0x68,
            0xED,
            0x1C,
            0x77,
            0xA7,
            0x0E,
            0x0A,
            0x9E,
            0x47,
            0x82,
            0xA4,
            0x57,
            0x24,
        ]

        self.payload_size = payload_size = 300  # bytes
        self.data = data = [0] * 4 + \
            [random.getrandbits(8) for i in range(payload_size)]
        self.gain = gain = .001  # LMS gain
        self.corr_thresh = corr_thresh = 3e6
        self.num_taps = num_taps = 16

    def tearDown(self):
        self.tb = None

    def transform(self, src_data, const, alg):
        src = blocks.vector_source_c(src_data, False)
        leq = digital.linear_equalizer(
            4,
            1,
            alg,
            True, 
            [],
            '')
        dst = blocks.vector_sink_c()
        self.tb.connect(src, leq, dst)
        self.tb.run()
        return dst.data()

    def test_001_identity_lms(self):
        # Constant modulus signal so no adjustments
        const = digital.constellation_qpsk()
        src_data = const.points() * 1000
        alg = digital.adaptive_algorithm_lms(const, .1).base()

        N = 100  # settling time
        expected_data = src_data[N:]
        result = self.transform(src_data, const, alg)[N:]

        N = -500
        self.assertComplexTuplesAlmostEqual(expected_data[N:], result[N:], 5)

    def test_002_identity_cma(self):
        # Constant modulus signal so no adjustments
        const = digital.constellation_qpsk()
        src_data = const.points() * 1000
        alg = digital.adaptive_algorithm_cma(const, .001, 4).base()

        N = 100  # settling time
        expected_data = src_data[N:]
        result = self.transform(src_data, const, alg)[N:]

        N = -500
        self.assertComplexTuplesAlmostEqual(expected_data[N:], result[N:], 5)

    def test_qpsk_3tap_lms_training(self):
        # set up fg
        gain = 0.01  # LMS gain
        num_taps = 16
        num_samp = 2000
        num_test = 500
        cons = digital.constellation_qpsk().base()
        rxmod = digital.generic_mod(
            cons, False, self.sps, True, self.eb, False, False)
        modulated_sync_word_pre = digital.modulate_vector_bc(
            rxmod.to_basic_block(), self.preamble + self.preamble, [1])
        # compensate for the RRC filter delay
        modulated_sync_word = modulated_sync_word_pre[86:(512 + 86)]
        corr_max = numpy.abs(
            numpy.dot(
                modulated_sync_word,
                numpy.conj(modulated_sync_word)))
        corr_calc = self.corr_thresh / (corr_max * corr_max)
        preamble_symbols = self.map_symbols_to_constellation(
            self.unpack_values(self.preamble, 8, 2), cons)

        alg = digital.adaptive_algorithm_lms(cons, gain).base()
        evm = digital.meas_evm_cc(cons, digital.evm_measurement_t.EVM_PERCENT)
        leq = digital.linear_equalizer(
            num_taps,
            self.sps,
            alg,
            False,
            preamble_symbols,
            'corr_est')
        correst = digital.corr_est_cc(
            modulated_sync_word,
            self.sps,
            12,
            corr_calc,
            digital.THRESHOLD_ABSOLUTE)
        constmod = digital.generic_mod(
            constellation=cons,
            differential=False,
            samples_per_symbol=4,
            pre_diff_code=True,
            excess_bw=0.35,
            verbose=False,
            log=False)
        chan = channels.channel_model(
            noise_voltage=0.0,
            frequency_offset=0.0,
            epsilon=1.0,
            taps=(1.0 + 1.0j, 0.63 - .22j, -.1 + .07j),
            noise_seed=0,
            block_tags=False)
        vso = blocks.vector_source_b(self.preamble + self.data, True, 1, [])
        head = blocks.head(gr.sizeof_float * 1, num_samp)
        vsi = blocks.vector_sink_f()

        self.tb.connect(vso, constmod, chan, correst, leq, evm, head, vsi)
        self.tb.run()

        # look at the last 1000 samples, should converge quickly, below 5% EVM
        upper_bound = list(20.0 * numpy.ones((num_test,)))
        lower_bound = list(0.0 * numpy.zeros((num_test,)))
        output_data = vsi.data()
        output_data = output_data[-num_test:]
        self.assertLess(output_data, upper_bound)
        self.assertGreater(output_data, lower_bound)


if __name__ == '__main__':
    gr_unittest.run(qa_linear_equalizer)
