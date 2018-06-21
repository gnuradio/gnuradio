""" The file for testing the gr-modtool scripts """
import shutil
import tempfile
from os import path
import unittest
import warnings

from gnuradio.modtool.core import *

class TestModToolCore(unittest.TestCase):
    """ The tests for the modtool core """
    def __init__(self, *args, **kwargs):
        super(TestModToolCore, self).__init__(*args, **kwargs)
        self.f_add = False
        self.f_newmod = False

    @classmethod
    def setUpClass(cls):
        """ create a temporary directory """
        cls.test_dir = tempfile.mkdtemp()

    @classmethod
    def tearDownClass(cls):
        """ remove the directory after the test """
        shutil.rmtree(cls.test_dir)

    def setUp(self):
        """ create a new module and block before every test """
        try:
            warnings.simplefilter("ignore", ResourceWarning)
            args = {'module_name':'howto', 'directory': self.test_dir}
            ModToolNewModule(**args).run()
        except (TypeError, ModToolException):
            self.f_newmod = True
        else:
            try:
                args = {'blockname':'square_ff', 'block_type':'general',
                        'lang':'cpp', 'directory': self.test_dir + '/gr-howto',
                        'add_python_qa': True}
                ModToolAdd(**args).run()
            except (TypeError, ModToolException):
                self.f_add = True

    def tearDown(self):
        """ removes the created module """
        # Required, else the new-module directory command
        # in setup with throw exception after first test
        ## cannot remove if directory is not created
        if not self.f_newmod:
            rmdir = self.test_dir + '/gr-howto'
            shutil.rmtree(rmdir)

    def test_newmod(self):
        """ Tests for the API function newmod """
        ## Tests for proper exceptions ##
        test_dict = {}
        test_dict['directory'] = self.test_dir
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
        self.assertTrue(path.isdir(self.test_dir+'/gr-test'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/lib'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/python'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/include'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/docs'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/cmake'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test/swig'))
        self.assertTrue(path.exists(self.test_dir+'/gr-test/CMakeLists.txt'))

        ## The check for object instantiation ##
        test_obj = ModToolNewModule()
        # module name not specified
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['modname'] = 'howto'
        test_obj.directory = self.test_dir
        # directory already exists
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['modname'] = 'test1'
        test_obj.run()
        self.assertTrue(path.isdir(self.test_dir+'/gr-test1'))
        self.assertTrue(path.isdir(self.test_dir+'/gr-test1/lib'))
        self.assertTrue(path.exists(self.test_dir+'/gr-test1/CMakeLists.txt'))

    def test_add(self):
        """ Tests for the API function add """
        ## skip tests if newmod command wasn't successful
        if self.f_newmod:
            raise unittest.SkipTest("setUp for API function 'add' failed")

        ## Tests for proper exceptions ##
        test_dict = {}
        test_dict['directory'] = self.test_dir + '/gr-howto'
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

        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/qa_add_ff.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/add_ff_impl.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/grc/howto_add_ff.xml'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/include/howto/add_ff.h'))

        ## The check for object instantiation ##
        test_obj = ModToolAdd()
        test_obj.dir = self.test_dir + '/gr-howto'
        # missing blocktype, lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['blocktype'] = 'general'
        # missing lang, blockname
        self.assertRaises(ModToolException, test_obj.run)
        test_obj.info['lang'] = 'cpp'
        test_obj.info['blockname'] = 'mul_ff'
        test_obj.run()
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/mul_ff_impl.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/mul_ff_impl.h'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/grc/howto_mul_ff.xml'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/include/howto/mul_ff.h'))

    def test_rename(self):
        """ Tests for the API function rename """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'rename' failed")
        test_dict = {}
        test_dict['directory'] = self.test_dir+'/gr-howto'
        # Missing 2 arguments blockname, new_name
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)
        test_dict['blockname'] = 'square_ff'
        # Missing argument new_name
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)
        test_dict['new_name'] = '//#'
        # Invalid new block name!
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)
        test_dict['new_name'] = None
        # New Block name not specified
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        ## Some tests for checking the renamed files ##
        test_dict['new_name'] = 'div_ff'
        ModToolRename(**test_dict).run()
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/div_ff_impl.h'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/div_ff_impl.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/python/qa_div_ff.py'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/grc/howto_div_ff.xml'))

        ## The check for object instantiation ##
        test_obj = ModToolRename()
        test_obj.info['oldname'] = 'div_ff'
        test_obj.info['newname'] = 'sub_ff'
        test_obj.run()
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/sub_ff_impl.h'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/sub_ff_impl.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/python/qa_sub_ff.py'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/grc/howto_sub_ff.xml'))

    def test_remove(self):
        """ Tests for the API function remove """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'remove' failed")
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)
        test_dict['directory'] = self.test_dir+'/gr-howto'
        self.assertRaises(ModToolException, ModToolRename(**test_dict).run)

        ## Some tests to check blocks are not removed with different blocknames ##
        test_dict['blockname'] = 'div_ff'
        ModToolRemove(**test_dict).run()
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/square_ff_impl.h'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/lib/square_ff_impl.cc'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/python/qa_square_ff.py'))
        self.assertTrue(path.exists(self.test_dir+'/gr-howto/grc/howto_square_ff.xml'))

        ## Some tests for checking the non-existence of removed files ##
        test_dict['blockname'] = 'square_ff'
        ModToolRemove(**test_dict).run()
        self.assertTrue(not path.exists(self.test_dir+'/gr-howto/lib/square_ff_impl.h'))
        self.assertTrue(not path.exists(self.test_dir+'/gr-howto/lib/square_ff_impl.cc'))
        self.assertTrue(not path.exists(self.test_dir+'/gr-howto/python/qa_square_ff.py'))
        self.assertTrue(not path.exists(self.test_dir+'/gr-howto/grc/howto_square_ff.xml'))

    def test_makexml(self):
        """ Tests for the API function makexml """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'makexml' failed")
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolMakeXML(**test_dict).run)
        test_dict['directory'] = self.test_dir+'/gr-howto'
        self.assertRaises(ModToolException, ModToolMakeXML(**test_dict).run)

        ## Some tests to check if the command reuns ##
        test_dict['blockname'] = 'square_ff'
        ModToolMakeXML(**test_dict).run()

    def test_disable(self):
        """ Tests for the API function disable """
        if self.f_newmod or self.f_add:
            raise unittest.SkipTest("setUp for API function 'disable' failed")
        test_dict = {}
        # missing argument blockname
        self.assertRaises(ModToolException, ModToolDisable(**test_dict).run)
        test_dict['directory'] = self.test_dir+'/gr-howto'
        self.assertRaises(ModToolException, ModToolDisable(**test_dict).run)

        ## Some tests to check if the command reuns ##
        test_dict['blockname'] = 'square_ff'
        ModToolDisable(**test_dict).run()

if __name__ == '__main__':
    unittest.main()
