#
# Copyright 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" The file for testing the gr-modtool scripts """


import os
import shutil
import tempfile
import unittest
import warnings
from os import path
try:
    from pylint.epylint import py_run
    skip_pylint_test = False
except:
    skip_pylint_test = True

try:  # for debugging, full path must be given
    from gnuradio.modtool.core import ModToolNewModule
    from gnuradio.modtool.core import ModToolAdd
    from gnuradio.modtool.core import ModToolDisable
    from gnuradio.modtool.core import ModToolException
    from gnuradio.modtool.core import ModToolMakeYAML
    from gnuradio.modtool.core import ModToolRename
    from gnuradio.modtool.core import ModToolRemove
except:
    from modtool.core import ModToolNewModule
    from modtool.core import ModToolAdd
    from modtool.core import ModToolDisable
    from modtool.core import ModToolException
    from modtool.core import ModToolMakeYAML
    from modtool.core import ModToolRename
    from modtool.core import ModToolRemove


class TestModToolCore(unittest.TestCase):
    """ The tests for the modtool core """

    def __init__(self, *args, **kwargs):
        super(TestModToolCore, self).__init__(*args, **kwargs)
        self.f_add = False
        self.f_newmod = False
        self.srcdir = path.abspath(path.join(path.dirname(
            path.realpath(__file__)), '../templates/gr-newmod'))

    @classmethod
    def setUpClass(cls):
        """ create a temporary directory """
        cls.start_dir = os.getcwd()
        cls.test_dir = tempfile.mkdtemp()

    @classmethod
    def tearDownClass(cls):
        """ remove the directory after the test """
        os.chdir(cls.start_dir)
        shutil.rmtree(cls.test_dir)

    def setUp(self):
        """ create a new module and block before every test """
        try:
            warnings.simplefilter("ignore", ResourceWarning)
            args = {'module_name': 'howto',
                    'directory': self.test_dir,
                    'srcdir': self.srcdir}
            ModToolNewModule(**args).run()
        except (TypeError, ModToolException):
            self.f_newmod = True
        else:
            try:
                args = {'blockname': 'square_ff', 'block_type': 'general',
                        'lang': 'cpp', 'directory': self.test_dir + '/gr-howto',
                        'add_python_qa': True}
                ModToolAdd(**args).run()
            except (TypeError, ModToolException):
                self.f_add = True

    def tearDown(self):
        """ removes the created module """
        # Required, else the new-module directory command
        # in setup will throw exception after first test
        # cannot remove if directory is not created
        if not self.f_newmod:
            os.chdir(self.start_dir)
            rmdir = self.test_dir + '/gr-howto'
            shutil.rmtree(rmdir)

    def test_newmod(self):
        """ Tests for the API function newmod """
        ## Tests for proper exceptions ##
        test_dict = {'directory': self.test_dir,
                     'srcdir': self.srcdir}
        # module name not specified
        self.assertRaises(ModToolException, ModToolNewModule(**test_dict).run)
        test_dict['module_name'] = 'howto'
        # expected module_name as a string instead of dict
        self.assertRaises(TypeError, ModToolNewModule(test_dict).run)
        # directory already exists
        # will not be raised if the command in setup failed
        self.assertRaises(ModToolException, ModToolNewModule(**test_dict).run)

        ## Some tests for checking the created directory, sub-directories and files ##
        test_dict['module_name'] = 'test'
        ModToolNewModule(**test_dict).run()
        module_dir = path.join(self.test_dir, 'gr-test')
        self.assertTrue(path.isdir(module_dir))
        self.assertTrue(path.isdir(path.join(module_dir, 'lib')))
        self.assertTrue(path.isdir(path.join(module_dir, 'python')))
        self.assertTrue(path.isdir(path.join(module_dir, 'include')))
        self.assertTrue(path.isdir(path.join(module_dir, 'docs')))
        self.assertTrue(path.isdir(path.join(module_dir, 'cmake')))
        self.assertTrue(path.exists(path.join(module_dir, 'CMakeLists.txt')))

        ## The check for object instantiation ##
        test_obj = ModToolNewModule(srcdir=self.srcdir)
        # module name not specified
        with self.assertRaises(ModToolException) as context_manager:
            test_obj.run()
        test_obj.info['modname'] = 'howto'
        test_obj.directory = self.test_dir
        # directory already exists
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['modname'] = 'test1'
        test_obj.run()
        self.assertTrue(path.isdir(self.test_dir + '/gr-test1'))
        self.assertTrue(path.isdir(self.test_dir + '/gr-test1/lib'))
        self.assertTrue(path.exists(
            self.test_dir + '/gr-test1/CMakeLists.txt'))

    @unittest.skipIf(skip_pylint_test, 'pylint dependency missing, skip test')
    def test_pylint_newmod(self):
        """ Pylint tests for API function newmod """
        module_dir = path.join(self.test_dir, 'gr-test')
        ## pylint tests ##
        python_dir = path.join(module_dir, 'python', 'test')
        py_module = path.join(python_dir, 'mul_ff.py')
        (pylint_stdout, pylint_stderr) = py_run(py_module +
                                                ' --errors-only --disable=E0602', return_std=True)
        print(pylint_stdout.getvalue(), end='')
        py_module = path.join(python_dir, 'qa_mul_ff.py')
        (pylint_stdout, pylint_stderr) = py_run(
            py_module + ' --errors-only', return_std=True)
        print(pylint_stdout.getvalue(), end='')

    def test_add(self):
        """ Tests for the API function add """
        # skip tests if newmod command wasn't successful
        if self.f_newmod:
            raise unittest.SkipTest("setUp for API function 'add' failed")
        module_dir = path.join(self.test_dir, 'gr-howto')
        ## Tests for proper exceptions ##
        test_dict = {}
        test_dict['directory'] = module_dir
        # missing blockname, block_type, lang
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['blockname'] = 'add_ff'
        # missing arguments block_type, lang
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['block_type'] = 'general'
        # missing argument lang
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['lang'] = 'cxx'
        # incorrect language
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['lang'] = 'cpp'
        test_dict['add_cpp_qa'] = 'Wrong'
        # boolean is expected for add_cpp_qa
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['add_cpp_qa'] = True
        test_dict['block_type'] = 'generaleee'
        # incorrect block type
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['block_type'] = 'general'
        test_dict['skip_lib'] = 'fail'
        # boolean value is expected for skip_lib, fails in instantiation
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)
        test_dict['skip_lib'] = True
        # missing relevant subdir
        self.assertRaises(ModToolException, ModToolAdd(**test_dict).run)

        ## Some tests for checking the created directory, sub-directories and files ##
        test_dict['skip_lib'] = False
        ModToolAdd(**test_dict).run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'qa_add_ff.cc')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'add_ff_impl.cc')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'grc', 'howto_add_ff.block.yml')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'include', 'gnuradio', 'howto', 'add_ff.h')))

        ## The check for object instantiation ##
        test_obj = ModToolAdd()
        test_obj.dir = module_dir
        # missing blocktype, lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['blocktype'] = 'general'
        # missing lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['lang'] = 'python'
        test_obj.info['blockname'] = 'mul_ff'
        test_obj.add_py_qa = True
        test_obj.run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'mul_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_mul_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'grc', 'howto_mul_ff.block.yml')))

    @unittest.skipIf(skip_pylint_test, 'pylint dependency missing, skip test')
    def test_pylint_add(self):
        """ Pylint tests for API function add """
        # skip tests if newmod command wasn't successful
        if self.f_newmod:
            raise unittest.SkipTest("setUp for API function 'add' failed")
        module_dir = path.join(self.test_dir, 'gr-howto')

        ## The check for object instantiation ##
        test_obj = ModToolAdd()
        test_obj.dir = module_dir
        # missing blocktype, lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['blocktype'] = 'general'
        # missing lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['lang'] = 'python'
        test_obj.info['blockname'] = 'mul_ff'
        test_obj.add_py_qa = True
        test_obj.run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'mul_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_mul_ff.py')))

        ## pylint tests ##
        python_dir = path.join(module_dir, 'python', 'howto')
        py_module = path.join(python_dir, 'mul_ff.py')
        (pylint_stdout, pylint_stderr) = py_run(py_module +
                                                ' --errors-only --disable=E0602', return_std=True)
        print(pylint_stdout.getvalue(), end='')
        py_module = path.join(python_dir, 'qa_mul_ff.py')
        (pylint_stdout, pylint_stderr) = py_run(
            py_module + ' --errors-only', return_std=True)
        print(pylint_stdout.getvalue(), end='')

    def test_rename(self):
        """ Tests for the API function rename """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'rename' failed")

        module_dir = path.join(self.test_dir, 'gr-howto')
        test_dict = {}
        test_dict['directory'] = module_dir
        # Missing 2 arguments blockname, new_name
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        test_dict['blockname'] = 'square_ff'
        # Missing argument new_name
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        test_dict['new_name'] = '//#'
        # Invalid new block name!
        expected_message = "Invalid new block name '//#': names can only contain letters, numbers and underscores"
        self.assertRaisesRegex(
            ModToolException, expected_message, ModToolRename(**test_dict).run)

        test_dict['new_name'] = 'non-alpha-chars-like-hyphen-are-not-allowed'
        # Invalid new block name!
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        test_dict['new_name'] = None
        # New Block name not specified
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        ## Some tests for checking the renamed files ##
        test_dict['new_name'] = 'div_ff'
        ModToolRename(**test_dict).run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'div_ff_impl.h')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'div_ff_impl.cc')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_div_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'grc', 'howto_div_ff.block.yml')))

        ## The check for object instantiation ##
        test_obj = ModToolRename()
        test_obj.info['oldname'] = 'div_ff'
        test_obj.info['newname'] = 'sub_ff'
        test_obj.run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'sub_ff_impl.h')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'sub_ff_impl.cc')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_sub_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'grc', 'howto_sub_ff.block.yml')))

    def test_remove(self):
        """ Tests for the API function remove """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'remove' failed")
        module_dir = path.join(self.test_dir, 'gr-howto')
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)
        test_dict['directory'] = module_dir
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        ## Some tests to check blocks are not removed with different blocknames ##
        test_dict['blockname'] = 'div_ff'
        ModToolRemove(**test_dict).run()
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'square_ff_impl.h')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'lib', 'square_ff_impl.cc')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_square_ff.py')))
        self.assertTrue(path.exists(
            path.join(module_dir, 'grc', 'howto_square_ff.block.yml')))

        ## Some tests for checking the non-existence of removed files ##
        test_dict['blockname'] = 'square_ff'
        ModToolRemove(**test_dict).run()
        self.assertTrue(not path.exists(
            path.join(module_dir, 'lib', 'square_ff_impl.h')))
        self.assertTrue(not path.exists(
            path.join(module_dir, 'lib', 'square_ff_impl.cc')))
        self.assertTrue(not path.exists(
            path.join(module_dir, 'python', 'howto', 'qa_square_ff.py')))
        self.assertTrue(not path.exists(
            path.join(module_dir, 'grc', 'howto_square_ff.block.yml')))

    def test_makeyaml(self):
        """ Tests for the API function makeyaml """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'makeyaml' failed")
        module_dir = path.join(self.test_dir, 'gr-howto')
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolMakeYAML(**test_dict).run)
        test_dict['directory'] = module_dir
        self.assertRaises(ModToolException, ModToolMakeYAML(**test_dict).run)

        ## Some tests to check if the command reuns ##
        test_dict['blockname'] = 'square_ff'
        ModToolMakeYAML(**test_dict).run()

    def test_disable(self):
        """ Tests for the API function disable """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'disable' failed")
        module_dir = path.join(self.test_dir, 'gr-howto')
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolDisable(**test_dict).run)
        test_dict['directory'] = module_dir
        self.assertRaises(ModToolException, ModToolDisable(**test_dict).run)

        ## Some tests to check if the command reuns ##
        test_dict['blockname'] = 'square_ff'
        ModToolDisable(**test_dict).run()


if __name__ == '__main__':
    unittest.main()
