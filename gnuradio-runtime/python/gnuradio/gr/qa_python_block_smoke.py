#!/usr/bin/env python
#
# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

"""Regression test for https://github.com/gnuradio/gnuradio/issues/8137.

Instantiating any pure-Python ``gr.sync_block`` in a flowgraph currently
segfaults in a C++ worker thread launched by ``top_block.start()``,
before any ``work()`` invocation. Reproduces across every 3.10.x package
checked (Ubuntu 22.04/24.04/25.04, Debian bookworm) on both ``arm64``
and ``amd64``, with numpy versions ranging from 1.21 to 2.2.

Note: this bug only manifests when the flowgraph is built and started
inside the same Python interpreter that initially imports the runtime.
Running the same flowgraph inside a ``multiprocessing.Process`` (which
forks after the parent has already imported ``gr``) does *not* trigger
the crash, so the test spawns a fresh interpreter via ``subprocess``
instead. The test passes when the underlying GR runtime bug is fixed.
"""

import subprocess
import sys
import textwrap

from gnuradio import gr_unittest


# Run inline (no Process/fork): build a one-Python-block flowgraph and
# start it. Exits 0 on success; SIGSEGV (exit -11 / 139) demonstrates
# the regression.
_REPRO_SOURCE = textwrap.dedent("""
    import time
    import numpy
    from gnuradio import blocks, gr

    class P(gr.sync_block):
        def __init__(self):
            gr.sync_block.__init__(
                self, name="p",
                in_sig=[numpy.complex64], out_sig=[numpy.complex64])
        def work(self, ii, oi):
            oi[0][:] = ii[0]
            return len(oi[0])

    tb = gr.top_block()
    src = blocks.vector_source_c([complex(x, x + 1) for x in range(65536)])
    src.set_repeat(True)
    throttle = blocks.throttle(gr.sizeof_gr_complex, 1e6)
    tb.connect(src, throttle, P(), blocks.null_sink(gr.sizeof_gr_complex))

    tb.start()
    time.sleep(0.3)
    tb.stop()
    tb.wait()
""")


class test_python_block_smoke(gr_unittest.TestCase):

    def test_001_start_with_python_block(self):
        """A flowgraph containing a Python sync_block must start without
        crashing the runtime."""
        result = subprocess.run(
            [sys.executable, "-c", _REPRO_SOURCE],
            timeout=20,
            capture_output=True,
        )

        # On a healthy runtime the child exits 0. On the regression it
        # crashes with SIGSEGV; subprocess reports this as -signal on
        # POSIX (returncode == -11) or 139 if propagated by a shell.
        self.assertEqual(
            result.returncode, 0,
            "child interpreter exited with returncode={} when running a "
            "flowgraph containing a Python sync_block (negative codes "
            "are POSIX signals; -11 is SIGSEGV). See "
            "https://github.com/gnuradio/gnuradio/issues/8137\n"
            "stderr:\n{}".format(
                result.returncode,
                result.stderr.decode(errors="replace")))


if __name__ == '__main__':
    gr_unittest.run(test_python_block_smoke)
