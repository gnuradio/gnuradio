#
# Copyright 2026 Aryan Thakkar
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Tests for the errors raised when header parsing lacks pygccxml or castxml """


import os
import tempfile
import unittest
from unittest import mock

try:
    from gnuradio.blocktool.core import parseheader_generic
    from gnuradio.blocktool.core.base import MissingDependencyError
except ImportError:
    from blocktool.core import parseheader_generic
    from blocktool.core.base import MissingDependencyError


class TestMissingDependencyError(unittest.TestCase):

    def setUp(self):
        self.tmpdir = tempfile.TemporaryDirectory()
        include_dir = os.path.join(self.tmpdir.name, 'include')
        os.mkdir(include_dir)
        header = os.path.join(include_dir, 'not_a_block.h')
        with open(header, 'w') as f:
            f.write('class not_a_block {};\n')
        self.parser = parseheader_generic.GenericHeaderParser(file_path=header)

    def tearDown(self):
        self.tmpdir.cleanup()

    def test_simple_parser_failure_without_pygccxml(self):
        with mock.patch.object(parseheader_generic, 'PYGCCXML_AVAILABLE', False), \
                mock.patch.object(parseheader_generic, 'ParserCCBlock',
                                  side_effect=IOError, create=True):
            with self.assertRaises(MissingDependencyError):
                self.parser.get_header_info(['gr', 'test'])

    def test_castxml_not_found(self):
        utils = mock.Mock()
        utils.find_xml_generator.side_effect = Exception(
            "No c++ parser found. Please install castxml.")
        with mock.patch.object(parseheader_generic, 'PYGCCXML_AVAILABLE', True), \
                mock.patch.object(parseheader_generic, 'utils', utils, create=True):
            with self.assertRaises(MissingDependencyError):
                self.parser.get_header_info(['gr', 'test'])


if __name__ == '__main__':
    unittest.main()
