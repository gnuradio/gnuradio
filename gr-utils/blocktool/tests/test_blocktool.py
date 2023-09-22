#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" unittest for gr-blocktool api """


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

from blocktool import BlockHeaderParser
from blocktool.core.base import BlockToolException
from blocktool.core import Constants
from blocktool import RESULT_SCHEMA


class TestBlocktoolCore(unittest.TestCase):
    """ The Tests for blocktool core """

    def __init__(self, *args, **kwargs):
        super(TestBlocktoolCore, self).__init__(*args, **kwargs)
        self.module = os.path.abspath(os.path.join(os.path.dirname(__file__),
                                                   '../../../../gr-analog'))
        self.test_dir = os.path.abspath(os.path.join(self.module,
                                                     'include/gnuradio/analog'))

    def is_int(self, number):
        """
        Check for int conversion
        """
        try:
            int(number)
            return True
        except ValueError:
            return False

    @classmethod
    def setUpClass(cls):
        """ create a temporary Blocktool object """
        try:
            warnings.simplefilter("ignore", ResourceWarning)
        except NameError:
            pass
        test_path = {}
        target_file = os.path.abspath(os.path.join(os.path.dirname(
            __file__), '../../../../gr-analog/include/gnuradio/analog', 'agc2_cc.h'))
        test_path['file_path'] = target_file
        cls.test_obj = BlockHeaderParser(**test_path).get_header_info()

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_blocktool_exceptions(self):
        """
        tests for blocktool exceptions
        """
        # test for non-existent header or invalid headers
        test_dict = {}
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.test_dir, 'sample.h'))
        with self.assertRaises(BlockToolException):
            BlockHeaderParser(**test_dict).run_blocktool()
        # test for invalid header file
        test_dict['file_path'] = os.path.abspath(
            os.path.join(self.test_dir, 'CMakeLists.txt'))
        if not os.path.basename(test_dict['file_path']).endswith('.h'):
            with self.assertRaises(BlockToolException):
                BlockHeaderParser(**test_dict).run_blocktool()

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_namespace(self):
        """ test for header namespace """
        module_name = os.path.basename(self.module)
        self.assertEqual(self.test_obj['namespace'][0], 'gr')
        self.assertEqual(self.test_obj['namespace'][1], module_name.split('-')[-1])

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_io_signature(self):
        """ test for io_signature """
        input_signature = self.test_obj['io_signature']['input']['signature']
        output_signature = self.test_obj['io_signature']['output']['signature']
        valid_signature = False
        if input_signature and output_signature in Constants.SIGNATURE_LIST:
            valid_signature = True
        self.assertTrue(valid_signature)
        valid_io_stream = False
        input_max = self.test_obj['io_signature']['input']['max_streams']
        input_min = self.test_obj['io_signature']['input']['min_streams']
        output_max = self.test_obj['io_signature']['output']['max_streams']
        output_min = self.test_obj['io_signature']['output']['min_streams']
        if self.is_int(input_max) and self.is_int(input_min) and self.is_int(output_max) and self.is_int(output_min):
            valid_io_stream = True
        self.assertTrue(valid_io_stream)

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_message_port(self):
        """ test for message ports """
        input_port = self.test_obj['message_port']['input']
        output_port = self.test_obj['message_port']['output']
        valid_input_message_port = True
        valid_output_message_port = True
        if input_port:
            for port in input_port:
                if not port['id']:
                    valid_input_message_port = False
        if output_port:
            for port in output_port:
                if not port['id']:
                    valid_output_message_port = False
        self.assertTrue(valid_input_message_port)
        self.assertTrue(valid_output_message_port)

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_factory_signature(self):
        """ test for factory signature in the header """
        valid_factory_arg = True
        if self.test_obj['make']['arguments']:
            for arguments in self.test_obj['make']['arguments']:
                if not arguments['name'] or not arguments['dtype']:
                    valid_factory_arg = False
        self.assertTrue(valid_factory_arg)

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_methods(self):
        """ test for methods """
        valid_method = True
        if self.test_obj['methods']:
            for arguments in self.test_obj['methods']:
                if not arguments['name']:
                    valid_method = False
                if arguments['arguments_type']:
                    for args in arguments['arguments_type']:
                        if not args['name'] or not args['dtype']:
                            valid_method = False
        self.assertTrue(valid_method)

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_properties(self):
        """ test for properties """
        valid_properties = True
        if self.test_obj['properties']:
            for arguments in self.test_obj['properties']:
                if not arguments['name'] or not arguments['dtype']:
                    valid_properties = False
        self.assertTrue(valid_properties)

    @unittest.skipIf(SKIP_BLOCK_TEST, 'pygccxml not found, skipping this unittest')
    def test_result_format(self):
        """ test for parsed blocktool output format """
        valid_schema = False
        try:
            validate(instance=self.test_obj, schema=RESULT_SCHEMA)
            valid_schema = True
        except BlockToolException:
            raise BlockToolException
        self.assertTrue(valid_schema)


if __name__ == '__main__':
    unittest.main()
