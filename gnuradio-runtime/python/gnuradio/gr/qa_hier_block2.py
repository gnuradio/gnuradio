#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr_unittest
from gnuradio.gr.hier_block2 import _multiple_endpoints, _optional_endpoints


class test_hier_block2(gr_unittest.TestCase):

    def setUp(self):
        self.call_log = []
        self.Block = type("Block", (), {"to_basic_block": lambda bl: bl})

    def test_f(self, *args):
        """test doc"""
        self.call_log.append(args)

    multi = _multiple_endpoints(test_f)
    opt = _optional_endpoints(test_f)

    def test_000(self):
        self.assertEqual(self.multi.__doc__, "test doc")
        self.assertEqual(self.multi.__name__, "test_f")

    def test_001(self):
        b = self.Block()
        self.multi(b)
        self.assertEqual((b,), self.call_log[0])

    def test_002(self):
        b1, b2 = self.Block(), self.Block()
        self.multi(b1, b2)
        self.assertEqual([(b1, 0, b2, 0)], self.call_log)

    def test_003(self):
        b1, b2 = self.Block(), self.Block()
        self.multi((b1, 1), (b2, 2))
        self.assertEqual([(b1, 1, b2, 2)], self.call_log)

    def test_004(self):
        b1, b2, b3, b4 = [self.Block()] * 4
        self.multi(b1, (b2, 5), b3, (b4, 0))
        expected = [
            (b1, 0, b2, 5),
            (b2, 5, b3, 0),
            (b3, 0, b4, 0),
        ]
        self.assertEqual(expected, self.call_log)

    def test_005(self):
        with self.assertRaises(ValueError) as c:
            self.multi((self.Block(), 5))
        self.assertIsInstance(c.exception, ValueError)

    def test_006(self):
        with self.assertRaises(ValueError) as c:
            self.multi(self.Block(), (self.Block(), 5, 5))
        self.assertIsInstance(c.exception, ValueError)

    def test_007(self):
        b1, b2 = self.Block(), self.Block()
        self.opt(b1, "in", b2, "out")
        self.assertEqual([(b1, "in", b2, "out")], self.call_log)

    def test_008(self):
        f, b1, b2 = self.multi, self.Block(), self.Block()
        self.opt((b1, "in"), (b2, "out"))
        self.assertEqual([(b1, "in", b2, "out")], self.call_log)


if __name__ == '__main__':
    gr_unittest.run(test_hier_block2, "test_hier_block2.xml")

