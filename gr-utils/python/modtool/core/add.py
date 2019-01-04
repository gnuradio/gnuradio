#
# Copyright 2013-2014,2017,2018 Free Software Foundation, Inc.
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

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import logging

from ..tools import render_template, append_re_line_sequence, CMakeFileEditor
from ..templates import Templates
from .base import ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolAdd(ModTool):
    """ Add block to the out-of-tree module. """
    name = 'add'
    description = 'Add new block into a module.'
    block_types = ('sink', 'source', 'sync', 'decimator', 'interpolator',
                    'general', 'tagged_stream', 'hier', 'noblock')
    language_candidates = ('cpp', 'python', 'c++')

    def __init__(self, blockname=None, block_type=None, lang=None, copyright=None,
                 license_file=None, argument_list="", add_python_qa=False,
                 add_cpp_qa=False, skip_cmakefiles=False, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['blocktype'] = block_type
        self.info['lang'] = lang
        self.license_file = license_file
        self.info['copyrightholder'] = copyright
        self.info['arglist'] = argument_list
        self.add_py_qa = add_python_qa
        self.add_cc_qa = add_cpp_qa
        self.skip_cmakefiles = skip_cmakefiles

    def validate(self):
        """ Validates the arguments """
        ModTool._validate(self)
        if self.info['blocktype'] is None:
            raise ModToolException('Blocktype not specified.')
        if self.info['blocktype'] not in self.block_types:
            raise ModToolException('Invalid blocktype')
        if self.info['lang'] is None:
            raise ModToolException('Programming language not specified.')
        if self.info['lang'] not in self.language_candidates:
            raise ModToolException('Invalid programming language.')
        if self.info['blockname'] is None:
            raise ModToolException('Blockname not specified.')
        if not re.match('[a-zA-Z0-9_]+', self.info['blockname']):
            raise ModToolException('Invalid block name.')
        if not isinstance(self.add_py_qa, bool):
            raise ModToolException('Expected a boolean value for add_python_qa.')
        if not isinstance(self.add_cc_qa, bool):
            raise ModToolException('Expected a boolean value for add_cpp_qa.')
        if not isinstance(self.skip_cmakefiles, bool):
            raise ModToolException('Expected a boolean value for skip_cmakefiles.')

    def assign(self):
        if self.info['lang'] == 'c++':
            self.info['lang'] = 'cpp'
        if ((self.skip_subdirs['lib'] and self.info['lang'] == 'cpp')
                or (self.skip_subdirs['python'] and self.info['lang'] == 'python')):
            raise ModToolException('Missing or skipping relevant subdir.')
        self.info['fullblockname'] = self.info['modname'] + '_' + self.info['blockname']
        if not self.license_file:
            if self.info['copyrightholder'] is None:
                self.info['copyrightholder'] = '<+YOU OR YOUR COMPANY+>'
        self.info['license'] = self.setup_choose_license()
        if (self.info['blocktype'] in ('noblock') or self.skip_subdirs['python']):
            self.add_py_qa = False
        if not self.info['lang'] == 'cpp':
            self.add_cc_qa = False
        if self.info['version'] == 'autofoo' and not self.skip_cmakefiles:
            self.skip_cmakefiles = True

    def setup_choose_license(self):
        """ Select a license by the following rules, in this order:
        1) The contents of the file given by --license-file
        2) The contents of the file LICENSE or LICENCE in the modules
           top directory
        3) The default license. """
        if self.license_file is not None \
            and os.path.isfile(self.license_file):
            with open(self.license_file) as f:
                return f.read()
        elif os.path.isfile('LICENSE'):
            with open('LICENSE') as f:
                return f.read()
        elif os.path.isfile('LICENCE'):
            with open('LICENCE') as f:
                return f.read()
        elif self.info['is_component']:
            return Templates['grlicense']
        else:
            return Templates['defaultlicense'].format(**self.info)

    def _write_tpl(self, tpl, path, fname):
        """ Shorthand for writing a substituted template to a file"""
        path_to_file = os.path.join(path, fname)
        logger.info("Adding file '{}'...".format(path_to_file))
        with open(path_to_file, 'w') as f:
            f.write(render_template(tpl, **self.info))
        self.scm.add_files((path_to_file,))

    def run(self):
        """ Go, go, go. """
        # This portion will be covered by the CLI
        if not self.cli:
            self.validate()
            self.assign()

        has_swig = (
                self.info['lang'] == 'cpp'
                and not self.skip_subdirs['swig']
        )
        has_grc = False
        if self.info['lang'] == 'cpp':
            self._run_lib()
            has_grc = has_swig
        else: # Python
            self._run_python()
            if self.info['blocktype'] != 'noblock':
                has_grc = True
        if has_swig:
            self._run_swig()
        if self.add_py_qa:
            self._run_python_qa()
        if has_grc and not self.skip_subdirs['grc']:
            self._run_grc()

    def _run_cc_qa(self):
        " Add C++ QA files for 3.7 API if intructed from _run_lib"
        fname_qa_h  = 'qa_{}.h'.format(self.info['blockname'])
        fname_qa_cc = 'qa_{}.cc'.format(self.info['blockname'])
        self._write_tpl('qa_cpp', 'lib', fname_qa_cc)
        self._write_tpl('qa_h',   'lib', fname_qa_h)
        if self.skip_cmakefiles:
            return
        try:
            append_re_line_sequence(self._file['cmlib'],
                                    '\$\{CMAKE_CURRENT_SOURCE_DIR\}/qa_%s.cc.*\n' % self.info['modname'],
                                    '    ${CMAKE_CURRENT_SOURCE_DIR}/qa_%s.cc' % self.info['blockname'])
            append_re_line_sequence(self._file['qalib'],
                                    '#include.*\n',
                                    '#include "{}"'.format(fname_qa_h))
            append_re_line_sequence(self._file['qalib'],
                                    '(addTest.*suite.*\n|new CppUnit.*TestSuite.*\n)',
                                    '  s->addTest(gr::{}::qa_{}::suite());'.format(self.info['modname'],
                                                                                   self.info['blockname'])
                                    )
            self.scm.mark_files_updated((self._file['qalib'],))
        except IOError:
            logger.warning("Can't add C++ QA files.")

    def _run_lib(self):
        """ Do everything that needs doing in the subdir 'lib' and 'include'.
        - add .cc and .h files
        - include them into CMakeLists.txt
        - check if C++ QA code is req'd
        - if yes, create qa_*.{cc,h} and add them to CMakeLists.txt
        """
        fname_cc = None
        fname_h = None
        if self.info['version'] in ('37', '38'):
            fname_h = self.info['blockname'] + '.h'
            fname_cc = self.info['blockname'] + '.cc'
            if self.info['blocktype'] in ('source', 'sink', 'sync', 'decimator',
                                           'interpolator', 'general', 'hier', 'tagged_stream'):
                fname_cc = self.info['blockname'] + '_impl.cc'
                self._write_tpl('block_impl_h',   'lib', self.info['blockname'] + '_impl.h')
            self._write_tpl('block_impl_cpp', 'lib', fname_cc)
            self._write_tpl('block_def_h',    self.info['includedir'], fname_h)
        else: # Pre-3.7 or autotools
            fname_h  = self.info['fullblockname'] + '.h'
            fname_cc = self.info['fullblockname'] + '.cc'
            self._write_tpl('block_h36',   self.info['includedir'], fname_h)
            self._write_tpl('block_cpp36', 'lib',                    fname_cc)
        if self.add_cc_qa:
            if self.info['version'] == '37':
                self._run_cc_qa()
            elif self.info['version'] == '36':
                logger.warning("Warning: C++ QA files not supported for 3.6-style OOTs.")
            elif self.info['version'] == 'autofoo':
                logger.warning("Warning: C++ QA files not supported for autotools.")
        if not self.skip_cmakefiles:
            ed = CMakeFileEditor(self._file['cmlib'])
            cmake_list_var = '[a-z]*_?' + self.info['modname'] + '_sources'
            if not ed.append_value('list', fname_cc, to_ignore_start='APPEND ' + cmake_list_var):
                ed.append_value('add_library', fname_cc)
            ed.write()
            ed = CMakeFileEditor(self._file['cminclude'])
            ed.append_value('install', fname_h, to_ignore_end='DESTINATION[^()]+')
            ed.write()
            self.scm.mark_files_updated((self._file['cminclude'], self._file['cmlib']))

    def _run_swig(self):
        """ Do everything that needs doing in the subdir 'swig'.
        - Edit main *.i file
        """
        if self._get_mainswigfile() is None:
            logger.warning('Warning: No main swig file found.')
            return
        logger.info("Editing {}...".format(self._file['swig']))
        mod_block_sep = '/'
        if self.info['version'] == '36':
            mod_block_sep = '_'
        swig_block_magic_str = render_template('swig_block_magic', **self.info)
        with open(self._file['swig'], 'a') as f:
            f.write(swig_block_magic_str)
        include_str = '#include "{}{}{}.h"'.format(
                {True: 'gnuradio/' + self.info['modname'], False: self.info['modname']}[self.info['is_component']],
                mod_block_sep,
                self.info['blockname'])
        with open(self._file['swig'], 'r') as f:
            oldfile = f.read()
        if re.search('#include', oldfile):
            append_re_line_sequence(self._file['swig'], '^#include.*\n', include_str)
        else: # I.e., if the swig file is empty
            regexp = re.compile('^%\{\n', re.MULTILINE)
            oldfile = regexp.sub('%%{\n%s\n' % include_str, oldfile, count=1)
            with open(self._file['swig'], 'w') as f:
                f.write(oldfile)
        self.scm.mark_files_updated((self._file['swig'],))

    def _run_python_qa(self):
        """ Do everything that needs doing in the subdir 'python' to add
        QA code.
        - add .py files
        - include in CMakeLists.txt
        """
        fname_py_qa = 'qa_' + self.info['blockname'] + '.py'
        self._write_tpl('qa_python', self.info['pydir'], fname_py_qa)
        os.chmod(os.path.join(self.info['pydir'], fname_py_qa), 0o755)
        self.scm.mark_files_updated((os.path.join(self.info['pydir'], fname_py_qa),))
        if self.skip_cmakefiles or CMakeFileEditor(self._file['cmpython']).check_for_glob('qa_*.py'):
            return
        logger.info("Editing {}/CMakeLists.txt...".format(self.info['pydir']))
        with open(self._file['cmpython'], 'a') as f:
            f.write(
                'GR_ADD_TEST(qa_%s ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/%s)\n' % \
                (self.info['blockname'], fname_py_qa))
        self.scm.mark_files_updated((self._file['cmpython'],))

    def _run_python(self):
        """ Do everything that needs doing in the subdir 'python' to add
        a Python block.
        - add .py file
        - include in CMakeLists.txt
        - include in __init__.py
        """
        fname_py = self.info['blockname'] + '.py'
        self._write_tpl('block_python', self.info['pydir'], fname_py)
        append_re_line_sequence(self._file['pyinit'],
                                '(^from.*import.*\n|# import any pure.*\n)',
                                'from .{} import {}'.format(self.info['blockname'], self.info['blockname']))
        self.scm.mark_files_updated((self._file['pyinit'],))
        if self.skip_cmakefiles:
            return
        ed = CMakeFileEditor(self._file['cmpython'])
        ed.append_value('GR_PYTHON_INSTALL', fname_py, to_ignore_end='DESTINATION[^()]+')
        ed.write()
        self.scm.mark_files_updated((self._file['cmpython'],))

    def _run_grc(self):
        """ Do everything that needs doing in the subdir 'grc' to add
        a GRC bindings YAML file.
        - add .yml file
        - include in CMakeLists.txt
        """
        fname_grc = self.info['fullblockname'] + '.block.yml'
        self._write_tpl('grc_yml', 'grc', fname_grc)
        ed = CMakeFileEditor(self._file['cmgrc'], '\n    ')
        if self.skip_cmakefiles or ed.check_for_glob('*.yml'):
            return
        logger.info("Editing grc/CMakeLists.txt...")
        ed.append_value('install', fname_grc, to_ignore_end='DESTINATION[^()]+')
        ed.write()
        self.scm.mark_files_updated((self._file['cmgrc'],))
