#
# Copyright 2019 Free Software Foundation, Inc.
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
""" unittest for gr-blocktool api """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import unittest
import warnings
try:
    import pygccxml
    SKIP_BLOCK_TEST = False
except:
    SKIP_BLOCK_TEST = True

try:
    import apt
    CACHE = apt.cache.Cache()
    CACHE.open()

    PKG = CACHE['castxml']
    if PKG.is_installed:
        SKIP_BLOCK_TEST = False
    else:
        SKIP_BLOCK_TEST = True
except:
    SKIP_BLOCK_TEST = True

from jsonschema import validate

from blocktool.core.parseheader import BlockHeaderParser
from blocktool.core.base import BlockToolException
from blocktool.core import Constants
from blocktool.core.outputschema import RESULT_SCHEMA


class TestBlocktoolCore(unittest.TestCase):
    """ The Tests for blocktool core """

    def __init__(self, *args, **kwargs):
        super(TestBlocktoolCore, self).__init__(*args, **kwargs)
        self.current_folder = os.path.abspath(os.path.dirname(__file__))
        self.test_dir = os.path.abspath(os.path.join(
            self.current_folder, '../../../../gr-analog/include/gnuradio/analog'))
        self.target_file = os.path.abspath(
            os.path.join(self.test_dir, 'agc2_cc.h'))
        self.impl_dir = os.path.abspath(os.path.join(self.current_folder,
                                                     '../../../../gr-analog/lib'))
        self.modname = os.path.basename(os.path.dirname(
            os.path.dirname(os.path.dirname(os.path.dirname(self.target_file)))))
        self.gr = self.modname.split('-')[0]
        self.module = self.modname.split('-')[-1]

    def is_int(self, number):
        """
        Check for int conversion
        """
        try:
            int(number)
            return True
        except ValueError:
            return False

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not working, skipping whole unittest')
    def test_block_parser(self):
        """
        Tests for the block parser API
        """
        try:
            warnings.simplefilter("ignore", ResourceWarning)
        except NameError:
            pass

        # test for all the Blocktool Exceptions
        test_dict = {}
        # test for none type file_path
        test_dict['file_path'] = None
        with self.assertRaises(TypeError):
            BlockHeaderParser(**test_dict).run()
        # test for non-existent header
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.test_dir, 'sample.h'))
        with self.assertRaises(OSError):
            BlockHeaderParser(**test_dict).run()
        # test for valid header file
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.test_dir, 'CMakeLists.txt'))
        if not os.path.basename(test_dict['file_path']).endswith('.h'):
            with self.assertRaises(BlockToolException):
                BlockHeaderParser(**test_dict).run()
        # test for invalid header module
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.current_folder, '../../../../gr-utils/python/blocktool/__init__.py'))
        with self.assertRaises(BlockToolException):
            BlockHeaderParser(**test_dict).run()
        # test for invalid header file path
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.impl_dir, 'agc.h'))
        with self.assertRaises(OSError):
            BlockHeaderParser(**test_dict).run()
        # test for non-existent impl file
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.test_dir, 'agc.h'))
        with self.assertRaises(OSError):
            BlockHeaderParser(**test_dict).run()

        # test for returned parsed data
        # test for parser run
        test_dict['file_path'] = self.target_file
        test_obj = BlockHeaderParser(**test_dict).get_header_info()
        # test for namespace
        self.assertTrue(test_obj['namespace'][0] == self.gr)
        self.assertTrue(test_obj['namespace'][1] == self.module)
        # test for class
        self.assertTrue(test_obj['class'])
        # test for io_signature
        input_signature = test_obj['io_signature']['input']['signature']
        output_signature = test_obj['io_signature']['output']['signature']
        valid_signature = False
        if input_signature and output_signature in Constants.SIGNATURE_LIST:
            valid_signature = True
        self.assertTrue(valid_signature)
        valid_io_stream = False
        input_max = test_obj['io_signature']['input']['max_streams']
        input_min = test_obj['io_signature']['input']['min_streams']
        output_max = test_obj['io_signature']['output']['max_streams']
        output_min = test_obj['io_signature']['output']['min_streams']
        if self.is_int(input_max) and self.is_int(input_min) and self.is_int(output_max) and self.is_int(output_min):
            valid_io_stream = True
        self.assertTrue(valid_io_stream)
        # test for factory_signature
        valid_factory_arg = True
        if test_obj['make']['arguments']:
            for arguments in test_obj['make']['arguments']:
                if not arguments['name'] or not arguments['dtype']:
                    valid_factory_arg = False
        self.assertTrue(valid_factory_arg)
        # test for methods
        valid_method = True
        if test_obj['methods']:
            for arguments in test_obj['methods']:
                if not arguments['name']:
                    valid_method = False
                if arguments['arguments_type']:
                    for args in arguments['arguments_type']:
                        if not args['name'] or not args['dtype']:
                            valid_method = False
        self.assertTrue(valid_method)
        # test for properties
        valid_properties = True
        if test_obj['properties']:
            for arguments in test_obj['properties']:
                if not arguments['name'] or not arguments['dtype']:
                    valid_properties = False
        self.assertTrue(valid_properties)
        # Run the whole dict through JSON SCHEMA
        valid_schema = False
        try:
            validate(instance=test_obj, schema=RESULT_SCHEMA)
            valid_schema = True
        except BlockToolException:
            raise BlockToolException
        self.assertTrue(valid_schema)


if __name__ == '__main__':
    unittest.main()
