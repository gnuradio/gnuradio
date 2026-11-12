#!/usr/bin/env python
#
# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from PyQt6.QtWidgets import QApplication

from gnuradio import gr_unittest, qtgui


class qa_digitalnumbercontrol(gr_unittest.TestCase):

    def setUp(self):
        self.app = QApplication.instance() or QApplication([])
        self.notified = []
        self.control = qtgui.MsgDigitalNumberControl(
            lbl="Frequency",
            min_freq_hz=88e6,
            max_freq_hz=108e6,
            var_callback=self.notified.append,
        )
        self.control.setValue(100e6)
        self.app.processEvents()

    def tearDown(self):
        self.control = None

    def assert_state(self, min_freq, max_freq, value):
        self.app.processEvents()
        widget = self.control.numberControl
        self.assertEqual(widget.getMinFrequency(), min_freq)
        self.assertEqual(widget.getMaxFrequency(), max_freq)
        self.assertEqual(self.control.getFrequency(), value)

    def test_001_widen_keeps_value(self):
        self.control.set_min_freq(50e6)
        self.control.set_max_freq(137e6)
        self.assert_state(50e6, 137e6, 100e6)
        self.assertEqual(self.notified, [])

    def test_002_raised_min_clamps_and_notifies(self):
        self.control.set_min_freq(105e6)
        self.assert_state(105e6, 108e6, 105e6)
        self.assertEqual(self.notified, [105e6])

    def test_003_lowered_max_clamps_and_notifies(self):
        self.control.set_max_freq(95e6)
        self.assert_state(88e6, 95e6, 95e6)
        self.assertEqual(self.notified, [95e6])

    def test_004_switch_to_higher_band(self):
        # The new min arrives while the old max is still in place.
        self.control.set_min_freq(118e6)
        self.assert_state(118e6, 118e6, 118e6)
        self.control.set_max_freq(137e6)
        self.assert_state(118e6, 137e6, 118e6)

    def test_005_switch_to_lower_band(self):
        # The new max arrives while the old min is still in place.
        self.control.set_max_freq(70e6)
        self.assert_state(70e6, 70e6, 70e6)
        self.control.set_min_freq(50e6)
        self.assert_state(50e6, 70e6, 70e6)

    def test_006_max_updates_digit_count(self):
        self.control.set_max_freq(2000e6)
        self.assertEqual(self.control.numberControl.numDigitsInFreq, 10)
        self.assert_state(88e6, 2000e6, 100e6)


if __name__ == "__main__":
    gr_unittest.run(qa_digitalnumbercontrol)
