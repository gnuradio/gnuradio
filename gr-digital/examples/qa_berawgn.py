#!/usr/bin/env python
#
# Copyright 2026 Ritu Baskey
# SPDX-License-Identifier: GPL-3.0-or-later
#

"""Bounded regression tests for the BER example (CTest timeout: 30 seconds)."""

import importlib.util
import math
import unittest
from unittest.mock import patch

import numpy
from gnuradio import blocks, gr, gr_unittest

# These are optional GNU Radio dependencies, required by the example at import.
HAVE_EXAMPLE_DEPS = all(
    importlib.util.find_spec(name) is not None for name in ('scipy', 'matplotlib'))
if HAVE_EXAMPLE_DEPS:
    import berawgn


@unittest.skipUnless(HAVE_EXAMPLE_DEPS, 'berawgn requires scipy and matplotlib')
class test_berawgn(gr_unittest.TestCase):
    def setUp(self):
        self.bit_budget = patch.object(berawgn, 'N_BITS', 20000)
        self.bit_budget.start()
        self.addCleanup(self.bit_budget.stop)
        state = numpy.random.get_state()
        self.addCleanup(numpy.random.set_state, state)
        numpy.random.seed(42)

    def test_symbol_count(self):
        # Exercise the actual constructor: a float size is rejected by NumPy.
        with patch.object(numpy.random, 'randint', wraps=numpy.random.randint) as rng:
            fg = berawgn.BERAWGNSimu(0)
        self.assertEqual(fg.const.bits_per_symbol(), 2)
        self.assertEqual(rng.call_count, 1)
        self.assertEqual(rng.call_args[0], (0, 4, 10000))
        self.assertIsInstance(rng.call_args[0][2], int)

    def test_noise_amplitude(self):
        fg = berawgn.BERAWGNSimu(0)
        energy = numpy.mean(numpy.abs(fg.const.points())**2)
        self.assertAlmostEqual(energy, 4.0, places=6)
        self.assertAlmostEqual(fg.EbN0_to_noise_voltage(0), math.sqrt(2.0))
        self.assertAlmostEqual(fg.EbN0_to_noise_voltage(10), math.sqrt(0.2))

    def test_two_bit_errors(self):
        # 00, 01, 10, 11 against 00: four wrong bits, three wrong symbols.
        with patch.object(berawgn, 'N_BITS', 8):
            tb = gr.top_block()
            reference = blocks.vector_source_b([0, 1, 2, 3], False)
            received = blocks.vector_source_b([0, 0, 0, 0], False)
            errors = berawgn.BitErrors(2)
            sink = blocks.vector_sink_f()
            tb.connect(reference, (errors, 0))
            tb.connect(received, (errors, 1))
            tb.connect(errors, sink)
            tb.run()
        self.assertEqual(tuple(sink.data()), (0.5,))

    def test_noiseless_qpsk(self):
        with patch.object(berawgn, 'N_BITS', 8), \
                patch.object(numpy.random, 'randint', return_value=[0, 1, 2, 3]), \
                patch.object(berawgn.BERAWGNSimu, 'EbN0_to_noise_voltage', return_value=0):
            fg = berawgn.BERAWGNSimu(0)
            fg.run()
        self.assertEqual(tuple(fg.sink.data()), (0.0,))

    def test_ber_at_zero_db(self):
        fg = berawgn.BERAWGNSimu(0)
        fg.run()
        result = fg.sink.data()
        self.assertEqual(len(result), 1)  # Empty/incomplete output must not pass.
        theory = 0.5 * math.erfc(1.0)
        self.assertAlmostEqual(berawgn.berawgn(0), theory)
        # Binomial standard deviation of the fraction of 20000 independent bits.
        # Six sigma avoids relying on exact PRNG sequences across platforms,
        # while rejecting the old scaling (BER approximately 0.00234).
        tolerance = 6 * math.sqrt(theory * (1 - theory) / berawgn.N_BITS)
        self.assertAlmostEqual(result[0], theory, delta=tolerance)


if __name__ == '__main__':
    gr_unittest.run(test_berawgn)
