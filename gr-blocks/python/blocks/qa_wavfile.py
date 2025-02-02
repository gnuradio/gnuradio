#!/usr/bin/env python
#
# Copyright 2008,2010,2013,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

import os
import tempfile
import unittest
import wave
from os.path import getsize

g_in_file = os.path.join(os.getenv("srcdir"), "test_16bit_1chunk.wav")
g_in_file_normal = os.path.join(
    os.getenv("srcdir"),
    "test_16bit_1chunk_normal.wav")
g_in_file_mp3 = os.path.join(os.getenv("srcdir"), "test_no_info_frame.mp3")
g_extra_header_offset = 36
g_extra_header_len = 22


class test_wavefile(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_checkwavread(self):
        wf = blocks.wavfile_source(g_in_file)
        self.assertEqual(wf.sample_rate(), 8000)

    def test_002_checkwavcopy(self):
        infile = g_in_file
        outfile = "test_out.wav"

        wf_in = blocks.wavfile_source(infile)
        wf_out = blocks.wavfile_sink(outfile,
                                     wf_in.channels(),
                                     wf_in.sample_rate(),
                                     blocks.FORMAT_WAV,
                                     blocks.FORMAT_PCM_16)
        self.tb.connect(wf_in, wf_out)
        self.tb.run()
        wf_out.close()

        # Test file validity.
        try:
            with wave.open(infile, 'rb') as f:
                pass
            with wave.open(outfile, 'rb') as f:
                pass
        except BaseException:
            raise AssertionError('Invalid WAV file')

        # we're losing all extra header chunks
        self.assertEqual(
            getsize(infile) -
            g_extra_header_len,
            getsize(outfile))

        with open(infile, 'rb') as f:
            in_data = bytearray(f.read())
        with open(outfile, 'rb') as f:
            out_data = bytearray(f.read())

        os.remove(outfile)

        # Ignore size field:
        in_data[4:8] = b'\x00\x00\x00\x00'
        out_data[4:8] = b'\x00\x00\x00\x00'

        # cut extra header chunks from input file
        self.assertEqual(in_data[:g_extra_header_offset] +
                         in_data[g_extra_header_offset +
                                 g_extra_header_len:], out_data)

    def test_003_checkwav_append_copy(self):
        infile = g_in_file_normal
        outfile = "test_out_append.wav"

        # 1. Copy input to output
        from shutil import copyfile
        copyfile(infile, outfile)

        # 2. append copy
        wf_in = blocks.wavfile_source(infile)
        wf_out = blocks.wavfile_sink(outfile,
                                     wf_in.channels(),
                                     wf_in.sample_rate(),
                                     blocks.FORMAT_WAV,
                                     blocks.FORMAT_PCM_16,
                                     True)
        self.tb.connect(wf_in, wf_out)
        self.tb.run()
        wf_out.close()

        # 3. append halved copy
        self.tb.disconnect_all()
        wf_in = blocks.wavfile_source(infile)
        halver = blocks.multiply_const_ff(0.5)
        wf_out = blocks.wavfile_sink(outfile,
                                     wf_in.channels(),
                                     wf_in.sample_rate(),
                                     blocks.FORMAT_WAV,
                                     blocks.FORMAT_PCM_16,
                                     True)
        self.tb.connect(wf_in, halver, wf_out)
        self.tb.run()
        wf_out.close()

        # Test file validity and read data.
        try:
            # In
            with wave.open(infile, 'rb') as w_in:
                in_params = w_in.getparams()
                data_in = wav_read_frames(w_in)
            # Out
            with wave.open(outfile, 'rb') as w_out:
                out_params = w_out.getparams()
                data_out = wav_read_frames(w_out)
        except BaseException:
            raise AssertionError('Invalid WAV file')

        # Params must be equal except in size:
        expected_params = in_params._replace(nframes=3 * in_params.nframes)
        self.assertEqual(out_params, expected_params)

        # Part 1
        self.assertEqual(data_in, data_out[:len(data_in)])

        # Part 2
        self.assertEqual(data_in, data_out[len(data_in):2 * len(data_in)])

        # Part 3
        data_in_halved = [int(round(d / 2)) for d in data_in]
        self.assertEqual(data_in_halved, data_out[2 * len(data_in):])

        os.remove(outfile)

    def test_003_checkwav_append_non_existent_should_error(self):
        outfile = "no_file.wav"

        with self.assertRaisesRegex(RuntimeError, "Can't open WAV file."):
            blocks.wavfile_sink(
                outfile,
                1,
                44100,
                blocks.FORMAT_WAV,
                blocks.FORMAT_PCM_16,
                True)

        os.remove(outfile)

    def test_004_automatic_reopen_after_stop(self):
        infile = g_in_file_normal
        outfile = "test_out_append.wav"

        # 1. Copy input to output
        wf_in1 = blocks.wavfile_source(infile)
        wf_out = blocks.wavfile_sink(outfile,
                                     wf_in1.channels(),
                                     wf_in1.sample_rate(),
                                     blocks.FORMAT_WAV,
                                     blocks.FORMAT_PCM_16,
                                     False)
        self.tb.connect(wf_in1, wf_out)
        self.tb.run()

        # 2. lock and reconfigure to append the same copy again
        self.tb.lock()
        self.tb.disconnect(wf_in1, wf_out)
        wf_in2 = blocks.wavfile_source(infile)
        self.tb.connect(wf_in2, wf_out)
        self.tb.unlock()
        self.tb.run()

        wf_out.close()

        # Test file validity and read data.
        try:
            # In
            with wave.open(infile, 'rb') as w_in:
                in_params = w_in.getparams()
                data_in = wav_read_frames(w_in)
            # Out
            with wave.open(outfile, 'rb') as w_out:
                out_params = w_out.getparams()
                data_out = wav_read_frames(w_out)
        except BaseException:
            raise AssertionError('Invalid WAV file')

        # Params must be equal except in size:
        expected_params = in_params._replace(nframes=2 * in_params.nframes)
        self.assertEqual(out_params, expected_params)

        # Part 1
        self.assertEqual(data_in, data_out[:len(data_in)])

        # Part 2
        self.assertEqual(data_in, data_out[len(data_in):2 * len(data_in)])

        os.remove(outfile)

    def test_read_wav(self):
        expected_len = 4

        src = blocks.wavfile_source(g_in_file_normal)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, dst)
        self.tb.run()

        result = dst.data()

        self.assertEqual(len(result), expected_len)

    def test_read_wav_repeat(self):
        expected_len = 4
        repeats = 100

        src = blocks.wavfile_source(g_in_file_normal, repeat=True)
        head = blocks.head(gr.sizeof_float, expected_len * repeats)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, head, dst)
        self.tb.run()

        result = dst.data()

        self.assertEqual(len(result), expected_len * repeats)
        for i in range(1, repeats):
            offset = i * expected_len
            self.assertEqual(result[offset:offset + expected_len], result[0:expected_len])

    def test_read_wav_1024(self):
        for expected_len in range(1024, 32768, 1024):
            with tempfile.NamedTemporaryFile(suffix=".wav", delete=False) as tf:
                with wave.open(tf.name, "w") as wf:
                    wf.setnchannels(1)
                    wf.setsampwidth(2)
                    wf.setframerate(8000)
                    wf.writeframes(bytes([0, 0] * expected_len))

                self.tb = gr.top_block()
                src = blocks.wavfile_source(tf.name)
                dst = blocks.vector_sink_f()
                self.tb.connect(src, dst)
                self.tb.run()

                result = dst.data()

                del src
                del self.tb

                self.assertEqual(len(result), expected_len)

            os.unlink(tf.name)

    @unittest.skip("skipped due to bug in libsndfile < 1.2.1")
    def test_read_mp3(self):
        expected_len = 2304

        src = blocks.wavfile_source(g_in_file_mp3)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, dst)
        self.tb.run()

        result = dst.data()

        self.assertEqual(len(result), expected_len)

    @unittest.skip("skipped due to bug in libsndfile < 1.2.1")
    def test_read_mp3_repeat(self):
        expected_len = 2304
        repeats = 5

        src = blocks.wavfile_source(g_in_file_mp3, repeat=True)
        head = blocks.head(gr.sizeof_float, expected_len * repeats)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, head, dst)
        self.tb.run()

        result = dst.data()

        self.assertEqual(len(result), expected_len * repeats)
        for i in range(1, repeats):
            offset = i * expected_len
            self.assertEqual(result[offset:offset + expected_len], result[0:expected_len])


def wav_read_frames(w):
    import struct
    # grouper from itertools recipes.
    def grouper(iterable, n): return list(zip(* ([iter(iterable)] * n)))
    assert w.getsampwidth() == 2  # Assume 16 bits
    return [
        struct.unpack('h', bytes(frame_g))[0]
        for frame_g in grouper(w.readframes(w.getnframes()), 2)
    ]


if __name__ == '__main__':
    gr_unittest.run(test_wavefile)
