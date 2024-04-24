#!/usr/bin/env python3
#
# Copyright 2022 Johannes Demel
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr, gr_unittest, blocks
import numpy as np


class test_blockinterleaver_xx(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        np.random.seed(0x13375eed)

    def tearDown(self):
        self.tb = None

    def test_001_init(self):
        nbits = 16 * 2
        interleaver_indices = np.random.permutation(nbits)

        # Test if packed interleaver catches byte multiple errors.
        self.assertRaises(
            Exception,
            blocks.blockinterleaver_bb,
            [np.sort(interleaver_indices)[0:13], True, True],
        )

        # Test if interleaver detects malformed interleaver indices.
        self.assertRaises(
            Exception,
            blocks.blockinterleaver_bb,
            [interleaver_indices[0:13], True, True],
        )

        # Make sure output multiple is set correctly!
        interleaver = blocks.blockinterleaver_bb(interleaver_indices, True, True)
        self.assertEqual(interleaver.output_multiple(), nbits // 8)
        self.assertTrue(interleaver.is_packed())
        self.assertTrue(interleaver.interleaver_mode())
        self.assertTupleEqual(
            tuple(interleaver.interleaver_indices()), tuple(interleaver_indices)
        )

        self.assertEqual(
            np.array(interleaver.deinterleaver_indices()).size, interleaver_indices.size
        )

        interleaveru = blocks.blockinterleaver_bb(interleaver_indices, False, False)
        self.assertEqual(interleaveru.output_multiple(), nbits)
        self.assertFalse(interleaveru.is_packed())
        self.assertFalse(interleaveru.interleaver_mode())

    def test_002_bb_unpacked(self):
        nframes = 5
        nbits = 16 * 7
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_bb(interleaver_indices, True, False)

        data = np.random.randint(0, 2, nbits * nframes)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_b(data)
        snk = blocks.vector_sink_b()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_003_bb_packed(self):
        nframes = 20
        nbits = 16 * 17
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_bb(interleaver_indices, True, True)

        data = np.random.randint(0, 2, nbits * nframes)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        datavec = np.packbits(data)
        refvec = np.packbits(ref)
        src = blocks.vector_source_b(datavec)
        snk = blocks.vector_sink_b()
        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # check data
        resvec = np.array(snk.data())

        self.assertTupleEqual(tuple(resvec), tuple(refvec))

    def test_004_ff_unpacked(self):
        nframes = 5
        nbits = 16 * 7
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_ff(interleaver_indices, True)

        data = np.random.normal(size=nbits * nframes).astype(np.float32)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_f(data)
        snk = blocks.vector_sink_f()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_005_ff_unpacked_deinterleave(self):
        nframes = 5
        nbits = 16 * 17
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_ff(interleaver_indices, False)

        data = np.random.normal(size=nbits * nframes).astype(np.float32)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_f(ref)
        snk = blocks.vector_sink_f()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(data))

    def test_006_cc_unpacked(self):
        nframes = 5
        nbits = 16 * 7
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_cc(interleaver_indices, True)

        data = np.random.normal(size=nbits * nframes).astype(
            np.float32
        ) + 1.0j * np.random.normal(size=nbits * nframes).astype(np.float32)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_c(data)
        snk = blocks.vector_sink_c()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_007_ss_unpacked(self):
        nframes = 5
        nbits = 16 * 7
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_ss(interleaver_indices, True)

        data = np.random.normal(size=nbits * nframes).astype(np.int16)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_s(data)
        snk = blocks.vector_sink_s()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_008_ii_unpacked(self):
        nframes = 5
        nbits = 16 * 7
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_ii(interleaver_indices, True)

        data = np.random.normal(size=nbits * nframes).astype(np.int32)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_i(data)
        snk = blocks.vector_sink_i()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_009_bb_unpacked_huge(self):
        nframes = 2
        nbits = int(2 ** 16) + int(2 ** 15)
        interleaver_indices = np.random.permutation(nbits)

        interleaver = blocks.blockinterleaver_bb(interleaver_indices, True, False)

        data = np.random.randint(0, 2, nbits * nframes)
        ref = np.array([], dtype=data.dtype)
        for f in np.reshape(data, (nframes, -1)):
            ref = np.concatenate((ref, f[interleaver_indices]))
        src = blocks.vector_source_b(data)
        snk = blocks.vector_sink_b()

        self.tb.connect(src, interleaver, snk)
        # set up fg
        self.tb.run()
        # # check data
        res = np.array(snk.data())
        self.assertTupleEqual(tuple(res), tuple(ref))


if __name__ == "__main__":
    gr_unittest.run(test_blockinterleaver_xx)
