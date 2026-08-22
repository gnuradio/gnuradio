#!/usr/bin/env python
#
# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest


class test_realtime(gr_unittest.TestCase):

    def test_001_enable_realtime_scheduling_is_callable(self):
        # Whether real time scheduling can actually be enabled depends on the
        # privileges of whoever runs the suite, so this deliberately does not
        # assert RT_OK. It asserts that the call is reachable from Python and
        # reports a status, which is what was missing: the function had become
        # uncallable and nothing detected it.
        status = gr.enable_realtime_scheduling()

        self.assertIsInstance(status, gr.rt_status_t)
        self.assertIn(status,
                      (gr.RT_OK,
                       gr.RT_NOT_IMPLEMENTED,
                       gr.RT_NO_PRIVS,
                       gr.RT_OTHER_ERROR))

    def test_002_status_values_are_exposed(self):
        # The status values are what a caller checks the return against, so
        # they have to survive the bindings as well as the function does.
        self.assertEqual(int(gr.RT_OK), 0)
        self.assertEqual(int(gr.RT_NOT_IMPLEMENTED), 1)
        self.assertEqual(int(gr.RT_NO_PRIVS), 2)
        self.assertEqual(int(gr.RT_OTHER_ERROR), 3)


if __name__ == '__main__':
    gr_unittest.run(test_realtime)
