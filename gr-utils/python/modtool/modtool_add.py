#
# Copyright 2013 Free Software Foundation, Inc.
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
""" Module to add new blocks """

import os
import sys
import re
from optparse import OptionGroup

from util_functions import append_re_line_sequence, ask_yes_no
from cmakefile_editor import CMakeFileEditor
from modtool_base import ModTool
from templates import Templates
from code_generator import get_template
import Cheetah.Template

class ModToolAdd(ModTool):
    """ Add block to the out-of-tree module. """
    name = 'add'
    aliases = ('insert',)
    _block_types = ('sink', 'source', 'sync', 'decimator', 'interpolator',
                    'general', 'tagged_stream', 'hier', 'noblock')
    def __init__(self):
        ModTool.__init__(self)
        self._add_cc_qa = False
        self._add_py_qa = False

    def setup_parser(self):
        parser = ModTool.setup_parser(self)
        ogroup = OptionGroup(parser, "Add module options")
        ogroup.add_option("-t", "--block-type", type="choice",
                choices=self._block_types, default=None, help="One of %s." % ', '.join(self._block_types))
        ogroup.add_option("--license-file", type="string", default=None,
                help="File containing the license header for every source code file.")
        ogroup.add_option("--argument-list", type="string", default=None,
                help="The argument list for the constructor and make functions.")
        ogroup.add_option("--add-python-qa", action="store_true", default=None,
                help="If given, Python QA code is automatically added if possible.")
        ogroup.add_option("--add-cpp-qa", action="store_true", default=None,
                help="If given, C++ QA code is automatically added if possible.")
        ogroup.add_option("--skip-cmakefiles", action="store_true", default=False,
                help="If given, only source files are written, but CMakeLists.txt files are left unchanged.")
        ogroup.add_option("-l", "--lang", type="choice", choices=('cpp', 'c++', 'python'),
                default='cpp', help="Language (cpp or python)")
        parser.add_option_group(ogroup)
        return parser

    def setup(self):
        ModTool.setup(self)
        options = self.options
        self._info['blocktype'] = options.block_type
        if self._info['blocktype'] is None:
            while self._info['blocktype'] not in self._block_types:
                self._info['blocktype'] = raw_input("Enter code type: ")
                if self._info['blocktype'] not in self._block_types:
                    print 'Must be one of ' + str(self._block_types)
        self._info['lang'] = options.lang
        if self._info['lang'] == 'c++':
            self._info['lang'] = 'cpp'
        print "Language: %s" % {'cpp': 'C++', 'python': 'Python'}[self._info['lang']]

        if ((self._skip_subdirs['lib'] and self._info['lang'] == 'cpp')
             or (self._skip_subdirs['python'] and self._info['lang'] == 'python')):
            print "Missing or skipping relevant subdir."
            exit(1)

        if self._info['blockname'] is None:
            if len(self.args) >= 2:
                self._info['blockname'] = self.args[1]
            else:
                self._info['blockname'] = raw_input("Enter name of block/code (without module name prefix): ")
        if not re.match('[a-zA-Z0-9_]+', self._info['blockname']):
            print 'Invalid block name.'
            exit(2)
        print "Block/code identifier: " + self._info['blockname']
        self._info['fullblockname'] = self._info['modname'] + '_' + self._info['blockname']
        self._info['license'] = self.setup_choose_license()

        if options.argument_list is not None:
            self._info['arglist'] = options.argument_list
        else:
            self._info['arglist'] = raw_input('Enter valid argument list, including default arguments: ')

        if not (self._info['blocktype'] in ('noblock') or self._skip_subdirs['python']):
            self._add_py_qa = options.add_python_qa
            if self._add_py_qa is None:
                self._add_py_qa = ask_yes_no('Add Python QA code?', True)
        if self._info['lang'] == 'cpp':
            self._add_cc_qa = options.add_cpp_qa
            if self._add_cc_qa is None:
                self._add_cc_qa = ask_yes_no('Add C++ QA code?', not self._add_py_qa)
        if self._info['version'] == 'autofoo' and not self.options.skip_cmakefiles:
            print "Warning: Autotools modules are not supported. ",
            print "Files will be created, but Makefiles will not be edited."
            self.options.skip_cmakefiles = True


    def setup_choose_license(self):
        """ Select a license by the following rules, in this order:
        1) The contents of the file given by --license-file
        2) The contents of the file LICENSE or LICENCE in the modules
           top directory
        3) The default license. """
        if self.options.license_file is not None \
            and os.path.isfile(self.options.license_file):
            return open(self.options.license_file).read()
        elif os.path.isfile('LICENSE'):
            return open('LICENSE').read()
        elif os.path.isfile('LICENCE'):
            return open('LICENCE').read()
        elif self._info['is_component']:
            return Templates['grlicense']
        else:
            return Templates['defaultlicense']

    def _write_tpl(self, tpl, path, fname):
        """ Shorthand for writing a substituted template to a file"""
        print "Adding file '%s'..." % fname
        open(os.path.join(path, fname), 'w').write(get_template(tpl, **self._info))

    def run(self):
        """ Go, go, go. """
        has_swig = (
                self._info['lang'] == 'cpp'
                and not self._skip_subdirs['swig']
        )
        has_grc = False
        if self._info['lang'] == 'cpp':
            self._run_lib()
            has_grc = has_swig
        else: # Python
            self._run_python()
            if self._info['blocktype'] != 'noblock':
                has_grc = True
        if has_swig:
            self._run_swig()
        if self._add_py_qa:
            self._run_python_qa()
        if has_grc and not self._skip_subdirs['grc']:
            self._run_grc()

    def _run_lib(self):
        """ Do everything that needs doing in the subdir 'lib' and 'include'.
        - add .cc and .h files
        - include them into CMakeLists.txt
        - check if C++ QA code is req'd
        - if yes, create qa_*.{cc,h} and add them to CMakeLists.txt
        """
        def _add_qa():
            " Add C++ QA files for 3.7 API "
            fname_qa_h  = 'qa_%s.h'  % self._info['blockname']
            fname_qa_cc = 'qa_%s.cc' % self._info['blockname']
            self._write_tpl('qa_cpp', 'lib', fname_qa_cc)
            self._write_tpl('qa_h',   'lib', fname_qa_h)
            if not self.options.skip_cmakefiles:
                try:
                    append_re_line_sequence(self._file['cmlib'],
                                            '\$\{CMAKE_CURRENT_SOURCE_DIR\}/qa_%s.cc.*\n' % self._info['modname'],
                                            '    ${CMAKE_CURRENT_SOURCE_DIR}/qa_%s.cc' % self._info['blockname'])
                    append_re_line_sequence(self._file['qalib'],
                                            '#include.*\n',
                                            '#include "%s"' % fname_qa_h)
                    append_re_line_sequence(self._file['qalib'],
                                            '(addTest.*suite.*\n|new CppUnit.*TestSuite.*\n)',
                                            '  s->addTest(gr::%s::qa_%s::suite());' % (self._info['modname'],
                                                                                       self._info['blockname'])
                                            )
                except IOError:
                    print "Can't add C++ QA files."
        def _add_qa36():
            " Add C++ QA files for pre-3.7 API (not autotools) "
            fname_qa_cc = 'qa_%s.cc' % self._info['fullblockname']
            self._write_tpl('qa_cpp36', 'lib', fname_qa_cc)
            if not self.options.skip_cmakefiles:
                open(self._file['cmlib'], 'a').write(
                        str(
                            Cheetah.Template.Template(
                                Templates['qa_cmakeentry36'],
                                searchList={'basename': os.path.splitext(fname_qa_cc)[0],
                                            'upperbasename': os.path.splitext(fname_qa_cc)[0].upper(),
                                            'filename': fname_qa_cc,
                                            'modname': self._info['modname']
                                           }
                            )
                         )
                )
                ed = CMakeFileEditor(self._file['cmlib'])
                ed.remove_double_newlines()
                ed.write()
        fname_cc = None
        fname_h  = None
        if self._info['version']  == '37':
            fname_h  = self._info['blockname'] + '.h'
            fname_cc = self._info['blockname'] + '.cc'
            if self._info['blocktype'] in ('source', 'sink', 'sync', 'decimator',
                                           'interpolator', 'general', 'hier', 'tagged_stream'):
                fname_cc = self._info['blockname'] + '_impl.cc'
                self._write_tpl('block_impl_h',   'lib', self._info['blockname'] + '_impl.h')
            self._write_tpl('block_impl_cpp', 'lib', fname_cc)
            self._write_tpl('block_def_h',    self._info['includedir'], fname_h)
        else: # Pre-3.7 or autotools
            fname_h  = self._info['fullblockname'] + '.h'
            fname_cc = self._info['fullblockname'] + '.cc'
            self._write_tpl('block_h36',   self._info['includedir'], fname_h)
            self._write_tpl('block_cpp36', 'lib',                    fname_cc)
        if not self.options.skip_cmakefiles:
            ed = CMakeFileEditor(self._file['cmlib'])
            cmake_list_var = '[a-z]*_?' + self._info['modname'] + '_sources'
            if not ed.append_value('list', fname_cc, to_ignore_start='APPEND ' + cmake_list_var):
                ed.append_value('add_library', fname_cc)
            ed.write()
            ed = CMakeFileEditor(self._file['cminclude'])
            ed.append_value('install', fname_h, to_ignore_end='DESTINATION[^()]+')
            ed.write()
        if self._add_cc_qa:
            if self._info['version'] == '37':
                _add_qa()
            elif self._info['version'] == '36':
                _add_qa36()
            elif self._info['version'] == 'autofoo':
                print "Warning: C++ QA files not supported for autotools."

    def _run_swig(self):
        """ Do everything that needs doing in the subdir 'swig'.
        - Edit main *.i file
        """
        if self._get_mainswigfile() is None:
            print 'Warning: No main swig file found.'
            return
        print "Editing %s..." % self._file['swig']
        mod_block_sep = '/'
        if self._info['version'] == '36':
            mod_block_sep = '_'
        swig_block_magic_str = get_template('swig_block_magic', **self._info)
        open(self._file['swig'], 'a').write(swig_block_magic_str)
        include_str = '#include "%s%s%s.h"' % (
                {True: 'gnuradio/' + self._info['modname'], False: self._info['modname']}[self._info['is_component']],
                mod_block_sep,
                self._info['blockname'])
        if re.search('#include', open(self._file['swig'], 'r').read()):
            append_re_line_sequence(self._file['swig'], '^#include.*\n', include_str)
        else: # I.e., if the swig file is empty
            oldfile = open(self._file['swig'], 'r').read()
            regexp = re.compile('^%\{\n', re.MULTILINE)
            oldfile = regexp.sub('%%{\n%s\n' % include_str, oldfile, count=1)
            open(self._file['swig'], 'w').write(oldfile)

    def _run_python_qa(self):
        """ Do everything that needs doing in the subdir 'python' to add
        QA code.
        - add .py files
        - include in CMakeLists.txt
        """
        fname_py_qa = 'qa_' + self._info['blockname'] + '.py'
        self._write_tpl('qa_python', self._info['pydir'], fname_py_qa)
        os.chmod(os.path.join(self._info['pydir'], fname_py_qa), 0755)
        if self.options.skip_cmakefiles or CMakeFileEditor(self._file['cmpython']).check_for_glob('qa_*.py'):
            return
        print "Editing %s/CMakeLists.txt..." % self._info['pydir']
        open(self._file['cmpython'], 'a').write(
                'GR_ADD_TEST(qa_%s ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/%s)\n' % \
                  (self._info['blockname'], fname_py_qa))

    def _run_python(self):
        """ Do everything that needs doing in the subdir 'python' to add
        a Python block.
        - add .py file
        - include in CMakeLists.txt
        - include in __init__.py
        """
        fname_py = self._info['blockname'] + '.py'
        self._write_tpl('block_python', self._info['pydir'], fname_py)
        append_re_line_sequence(self._file['pyinit'],
                                '(^from.*import.*\n|# import any pure.*\n)',
                                'from %s import %s' % (self._info['blockname'], self._info['blockname']))
        if self.options.skip_cmakefiles:
            return
        ed = CMakeFileEditor(self._file['cmpython'])
        ed.append_value('GR_PYTHON_INSTALL', fname_py, to_ignore_end='DESTINATION[^()]+')
        ed.write()

    def _run_grc(self):
        """ Do everything that needs doing in the subdir 'grc' to add
        a GRC bindings XML file.
        - add .xml file
        - include in CMakeLists.txt
        """
        fname_grc = self._info['fullblockname'] + '.xml'
        self._write_tpl('grc_xml', 'grc', fname_grc)
        ed = CMakeFileEditor(self._file['cmgrc'], '\n    ')
        if self.options.skip_cmakefiles or ed.check_for_glob('*.xml'):
            return
        print "Editing grc/CMakeLists.txt..."
        ed.append_value('install', fname_grc, to_ignore_end='DESTINATION[^()]+')
        ed.write()

