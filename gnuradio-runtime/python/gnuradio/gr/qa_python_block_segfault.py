#!/usr/bin/env python
#
# Copyright 2026 Örjan Lundberg
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# Regression test for https://github.com/gnuradio/gnuradio/issues/8137
#
# A pure-Python gr.sync_block that was connected into a flowgraph but not
# otherwise retained by the caller (e.g. created as a local in __init__) used
# to be garbage-collected before the scheduler started. The C++ block_gateway
# keeps only a non-owning handle back to the Python block, so the worker thread
# dereferenced freed memory and segfaulted (SIGSEGV, exit code -11) in
# top_block.start(), before any work() call ran.
#
# The crash only manifests in a fresh interpreter where the block's reference
# count actually drops to zero -- it does not reproduce when the surrounding
# function (and thus the local reference) is still on the stack during run().
# We therefore launch a fresh interpreter via subprocess to actually trigger
# it, mirroring the manual reproducer in the issue.

import subprocess
import sys

from gnuradio import gr, gr_unittest

# Source for the child interpreter. The Python sync_block `P` is created as a
# local inside TB.__init__ and is *not* stored as an attribute, so before the
# fix its only strong reference disappears when __init__ returns.
_REPRO = """
import time
import numpy as np
from gnuradio import blocks, gr


class P(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self, name="p",
            in_sig=[np.complex64], out_sig=[np.complex64])

    def work(self, ii, oi):
        oi[0][:] = ii[0]
        return len(oi[0])


class TB(gr.top_block):
    def __init__(self):
        super().__init__()
        src_data = [complex(x, x + 1) for x in range(65536)]
        s = blocks.vector_source_c(src_data, True)
        t = blocks.throttle(gr.sizeof_gr_complex, 1e6)
        p, n = P(), blocks.null_sink(gr.sizeof_gr_complex)
        self.connect(s, t, p, n)


tb = TB()
tb.start()
time.sleep(0.3)
tb.stop()
tb.wait()
print("OK")
"""


class test_python_block_segfault(gr_unittest.TestCase):

    def test_start_does_not_segfault(self):
        # Run the reproducer in a fresh interpreter; before the fix this exits
        # with the SIGSEGV return code (-11 on POSIX).
        result = subprocess.run(
            [sys.executable, "-c", _REPRO],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=60,
        )
        self.assertEqual(
            result.returncode,
            0,
            msg="Python sync_block flowgraph crashed during start() "
            "(returncode={}):\n{}".format(
                result.returncode, result.stderr.decode(errors="replace")),
        )
        self.assertIn(b"OK", result.stdout)


if __name__ == '__main__':
    gr_unittest.run(test_python_block_segfault)
