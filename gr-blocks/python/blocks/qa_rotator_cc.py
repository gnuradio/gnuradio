#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import numpy as np
from numpy.random import uniform
import pmt


class qa_rotator_cc(gr_unittest.TestCase):

    def _setUp(self, n_samples=100, tag_inc_updates=True):
        """Base fixture: set up flowgraph and parameters"""
        self.n_samples = n_samples  # number of IQ samples to generate
        self.f_in = uniform(high=0.5)  # input frequency
        self.f_shift = uniform(high=0.5) - \
            self.f_in  # rotator's starting frequency

        # Input IQ samples
        in_angles = 2 * np.pi * np.arange(self.n_samples) * self.f_in
        in_samples = np.exp(1j * in_angles)

        # Rotator's starting phase increment
        phase_inc = 2 * np.pi * self.f_shift

        # Flowgraph:
        #
        #                             /-> Vector Sink
        # Vector Source -> Rotator --/
        #                            \
        #                             \-> Tag Debug
        #
        self.tb = gr.top_block()
        self.source = blocks.vector_source_c(in_samples)
        self.throttle = blocks.throttle(gr.sizeof_gr_complex, 2**16)
        self.rotator_cc = blocks.rotator_cc(phase_inc, tag_inc_updates)
        self.sink = blocks.vector_sink_c()
        self.tag_sink = blocks.tag_debug(gr.sizeof_gr_complex, "rot_phase_inc",
                                         "rot_phase_inc")
        self.tag_sink.set_save_all(True)
        self.tb.connect(self.source, self.throttle, self.rotator_cc, self.sink)
        self.tb.connect(self.rotator_cc, self.tag_sink)

    def setUp(self):
        self._setUp()

    def tearDown(self):
        self.tb = None

    def _post_phase_inc_cmd(self, new_phase_inc, offset=None):
        """Post phase increment update command to the rotator block"""
        cmd = pmt.make_dict()
        cmd = pmt.dict_add(cmd, pmt.intern("inc"),
                           pmt.from_double(new_phase_inc))
        if (offset is not None):
            cmd = pmt.dict_add(cmd, pmt.intern("offset"),
                               pmt.from_uint64(offset))
        self.rotator_cc.insert_tail(pmt.to_pmt("cmd"), cmd)

    def _assert_tags(self, expected_values, expected_offsets):
        """Check the tags received by the tag debug block"""
        tags = self.tag_sink.current_tags()
        expected_tags = list(zip(expected_values, expected_offsets))
        self.assertEqual(len(tags), len(expected_tags))

        for idx, (val, offset) in enumerate(expected_tags):
            self.assertAlmostEqual(pmt.to_double(tags[idx].value),
                                   val,
                                   places=5)
            self.assertEqual(tags[idx].offset, offset)

    def _compute_expected_samples(self, offsets, new_phase_incs):
        """Compute the samples expected on the rotator output

        Args:
            offsets (list): Sample offsets where the updates are expected.
            new_phase_incs (list): Rotator phase increments for each update.

        Returns:
            np.array: Array of expected IQ samples on the rotator output.

        """
        # Initial samples (preceding the first phase increment update)
        f_out = self.f_in + self.f_shift
        expected_angles = 2 * np.pi * np.arange(offsets[0]) * f_out
        expected_samples = np.exp(1j * expected_angles)

        for idx, (offset,
                  rot_phase_inc) in enumerate(zip(offsets, new_phase_incs)):
            prev_f_out = f_out  # Previous output frequency
            new_f_shift = rot_phase_inc / (2.0 * np.pi)  # New frequency shift
            f_out = self.f_in + new_f_shift  # New output frequency

            if idx == len(offsets) - 1:
                segment_len = self.n_samples - offset
            else:
                segment_len = offsets[idx + 1] - offset

            # NOTE: when the phase increment is updated, the assumption is that
            # the update occurs at the beginning of the sample period. Hence,
            # the new phase rotation pace is only observed in the succeeding
            # sample, at the beginning of the next sample period. In other
            # words, the new phase increment can only be observed after some
            # time goes by such that the rotator phase accumulates.
            #
            # For example, suppose there are ten samples, and the update occurs
            # at offset 5 (i.e., the sixth sample). Suppose also that the
            # initial phase increment is 0.1, and the new increment is 0.2. In
            # this case, we have the following sequence of angles:
            #
            #    [0, 0.1, 0.2, 0.3, 0.4, 0.5. 0.7, 0.9, 1.1, 1.3]
            #                             |     \
            #                             |      \--------\
            #                           update             \
            #                           applied         new phase
            #                                        increment observed
            #
            # Ultimately, the phase seen at the sample where the update is
            # applied is still determined by the preceding phase increment.
            expected_angles = expected_angles[-1] + (2 * np.pi * prev_f_out) +\
                (2 * np.pi * np.arange(segment_len) * f_out)
            expected_samples = np.concatenate(
                (expected_samples, np.exp(1j * expected_angles)))

        return expected_samples

    def _post_random_phase_inc_updates(self, offsets):
        """Update the phase increment randomly at chosen offsets

        Args:
            offsets (list): Sample offsets where the updates are to be applied.

        Returns:
            list: New phase increments defined randomly (list).

        """
        new_phase_incs = list()
        for offset in offsets:
            new_f_out = uniform(high=0.5)
            new_f_shift = new_f_out - self.f_in
            new_phase_inc = float(2 * np.pi * new_f_shift)
            new_phase_incs.append(new_phase_inc)
            self._post_phase_inc_cmd(new_phase_inc, offset)

        return new_phase_incs

    def test_freq_shift(self):
        """Complex sinusoid frequency shift"""
        f_out = self.f_in + self.f_shift  # expected output frequency

        # Expected IQ samples
        expected_angles = 2 * np.pi * np.arange(self.n_samples) * f_out
        expected_samples = np.exp(1j * expected_angles)

        self.tb.run()
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_scheduled_phase_inc_update(self):
        """Update the phase increment at a chosen offset via command message"""
        offset = int(self.n_samples / 2)  # when to update the phase increment
        offsets = [offset]
        new_phase_incs = self._post_random_phase_inc_updates(offsets)
        expected_samples = self._compute_expected_samples(
            offsets, new_phase_incs)

        self.tb.run()
        self._assert_tags(new_phase_incs, offsets)
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_scheduled_phase_inc_update_with_tagging_disabled(self):
        """Test a scheduled phase increment update without tagging the update

        Same as test_scheduled_phase_inc_update but with tagging disabled.

        """
        self._setUp(tag_inc_updates=False)

        offset = int(self.n_samples / 2)  # when to update the phase increment
        offsets = [offset]
        new_phase_incs = self._post_random_phase_inc_updates(offsets)
        expected_samples = self._compute_expected_samples(
            offsets, new_phase_incs)

        self.tb.run()
        tags = self.tag_sink.current_tags()
        self.assertEqual(len(tags), 0)
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_immediate_phase_inc_update(self):
        """Immediate phase increment update via command message

        In this test, the command message does not include the offset
        key. Hence, the rotator should update its phase increment immediately.

        """
        new_f_shift = uniform(high=0.5) - self.f_in  # rotator's new frequency
        new_phase_inc = float(2 * np.pi * new_f_shift)  # new phase increment
        f_out = self.f_in + new_f_shift  # output frequency after update

        # Post the phase increment command message to the rotator block
        self._post_phase_inc_cmd(new_phase_inc)

        # The rotator updates the increment immediately (on the first sample)
        expected_tag_offset = 0

        # Expected samples (all of them with the new frequency set via message)
        expected_angles = 2 * np.pi * np.arange(self.n_samples) * f_out
        expected_samples = np.exp(1j * expected_angles)

        self.tb.run()
        self._assert_tags([new_phase_inc], [expected_tag_offset])
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_zero_change_phase_inc_update(self):
        """Schedule a phase increment update that does not change anything

        If the scheduled phase increment update sets the same phase increment
        that is already active in the rotator block, there should be no effect
        on the output signal. Nevertheless, the rotator should still tag the
        update.

        """
        new_phase_inc = 2 * np.pi * self.f_shift  # no change
        offset = int(self.n_samples / 2)  # when to update the phase increment
        f_out = self.f_in + self.f_shift  # expected output frequency

        # Post the phase increment command message to the rotator block
        self._post_phase_inc_cmd(new_phase_inc, offset)

        # Expected IQ samples
        expected_angles = 2 * np.pi * np.arange(self.n_samples) * f_out
        expected_samples = np.exp(1j * expected_angles)

        self.tb.run()
        self._assert_tags([new_phase_inc], [offset])
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_consecutive_phase_inc_updates(self):
        """Test tagging of a few consecutive phase increment updates"""
        offsets = list(map(int, self.n_samples * np.arange(1, 4, 1) / 4))
        new_phase_incs = self._post_random_phase_inc_updates(offsets)
        expected_samples = self._compute_expected_samples(
            offsets, new_phase_incs)

        self.tb.run()
        self._assert_tags(new_phase_incs, offsets)
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_out_of_order_phase_inc_updates(self):
        """Test tagging of a few out-of-order phase increment updates

        The rotator should sort the increment updates and apply them in order.

        """
        n_updates = 3
        new_f_shifts = uniform(high=0.5, size=n_updates)  # new frequencies
        new_phase_incs = 2 * np.pi * new_f_shifts  # new phase increments
        offsets = self.n_samples * np.arange(1, 4, 1) / 4  # when to update

        # Post the phase increment command messages out of order
        for i in [0, 2, 1]:
            self._post_phase_inc_cmd(new_phase_incs[i], int(offsets[i]))

        self.tb.run()

        # Confirm they are received in order
        self._assert_tags(new_phase_incs, offsets)

        # Confirm the output waveform
        expected_samples = self._compute_expected_samples(
            offsets, new_phase_incs)
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_duplicate_phase_inc_updates(self):
        """Test multiple phase increment updates scheduled for the same sample

        The rotator block applies all updates scheduled for the same sample
        offset. In the end, only the last update shall take effect.

        """
        n_updates = 3
        offset = int(self.n_samples / 2)
        all_new_phase_incs = list()
        # Post the phase increment command messages
        for i in range(n_updates):
            new_phase_incs = self._post_random_phase_inc_updates([offset])
            expected_samples = self._compute_expected_samples([offset],
                                                              new_phase_incs)
            all_new_phase_incs.extend(new_phase_incs)

        self.tb.run()

        # All "n_updates" tags are expected to be present
        self._assert_tags(all_new_phase_incs, [offset] * n_updates)

        # However, only the last update takes effect on the rotated samples
        self.assertComplexTuplesAlmostEqual(self.sink.data(),
                                            expected_samples,
                                            places=4)

    def test_phase_inc_update_out_of_range(self):
        """Test phase increment update sent for an out-of-range offset"""
        self._setUp(n_samples=2**16)
        n_half_samples = int(self.n_samples / 2)

        # Schedule a phase increment update whose offset is initially
        # out-of-range due to being in the future
        new_phase_inc = 2 * np.pi * 0.1
        self._post_phase_inc_cmd(new_phase_inc, offset=n_half_samples)

        # Run the flowgraph and wait until the rotator block does some work
        self.tb.start()
        while (self.rotator_cc.nitems_written(0) == 0):
            pass

        # The out-of-range update (scheduled for the future) should not have
        # been applied yet, but it should not have been discarded either.
        self._assert_tags([], [])

        # Wait until at least the first half of samples have been processed
        while (self.rotator_cc.nitems_written(0) < n_half_samples):
            pass

        # Now, schedule an update that is out-of-range due to being in the past
        self._post_phase_inc_cmd(new_phase_inc, offset=0)

        # And run the flowgraph to completion
        self.tb.wait()

        # The increment update initially scheduled for the future should have
        # been applied when processing the second half of samples. In contrast,
        # the update scheduled for the past should have been discarded.
        self._assert_tags([new_phase_inc], [n_half_samples])


if __name__ == '__main__':
    gr_unittest.run(qa_rotator_cc)
