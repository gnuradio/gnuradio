#
# Copyright 2013-2014,2017-2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to add new blocks """


import os
import logging
import subprocess

from ..tools import render_template, append_re_line_sequence, CMakeFileEditor, CPPFileEditor, code_generator
from ..templates import Templates
from .base import ModTool, ModToolException, validate_name
from gnuradio.bindtool import BindingGenerator
from gnuradio import gr

logger = logging.getLogger(__name__)


def clang_format(s):
    try:
        p = subprocess.Popen(
            ["clang-format"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        out, err = p.communicate(s.encode('utf-8'))
        if p.returncode != 0:
            print(f"Failed to run clang-format: {err}")
            return s
        return out.decode('utf-8')
    except (RuntimeError, FileNotFoundError) as e:
        print(f"Failed to run clang-format: {e}")
        return s


class ModToolAdd(ModTool):
    """ Add block to the out-of-tree module. """
    name = 'add'
    description = 'Add new block into a module.'
    block_types = {
        "sink": "Sink block with inputs, but no stream outputs",
        "source": "Source block with outputs, but no stream inputs",
        "sync": "Block with synchronous 1:1 input-to-output",
        "decimator": "Block with synchronous N:1 input-to-output",
        "interpolator": "Block with synchronous 1:N input-to-output",
        "general": "General-purpose block type",
        "tagged_stream": "Block with input-to-output flow controlled by input stream tags (e.g. packetized streams)",
        "hier": "Hierarchical container block for other blocks; usually can be described by a flowgraph",
        "noblock": "C++ or Python class",
    }
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
        if self.info['blocktype'] == 'tagged_stream' and self.info['lang'] == 'python':
            raise ModToolException(
                'Tagged Stream Blocks for Python currently unsupported')
        if self.info['blockname'] is None:
            raise ModToolException('Blockname not specified.')
        validate_name('block', self.info['blockname'])
        if not isinstance(self.add_py_qa, bool):
            raise ModToolException(
                'Expected a boolean value for add_python_qa.')
        if not isinstance(self.add_cc_qa, bool):
            raise ModToolException('Expected a boolean value for add_cpp_qa.')
        if not isinstance(self.skip_cmakefiles, bool):
            raise ModToolException(
                'Expected a boolean value for skip_cmakefiles.')

    def assign(self):
        if self.info['lang'] == 'c++':
            self.info['lang'] = 'cpp'
        if ((self.skip_subdirs['lib'] and self.info['lang'] == 'cpp') or
                (self.skip_subdirs['python'] and self.info['lang'] == 'python')):
            raise ModToolException('Missing or skipping relevant subdir.')
        self.info['fullblockname'] = self.info['modname'] + \
            '_' + self.info['blockname']
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
        logger.info(f"Adding file '{path_to_file}'...")
        with open(path_to_file, 'w') as f:
            if fname.endswith('.cc') or fname.endswith('.h'):
                f.write(clang_format(render_template(tpl, **self.info)))
            else:
                f.write(render_template(tpl, **self.info))

        self.scm.add_files((path_to_file,))

    def run(self):
        """ Go, go, go. """

        # Some validation covered by the CLI - validate all parameters here
        self.validate()
        self.assign()

        has_pybind = (
            self.info['lang'] == 'cpp' and
            not self.skip_subdirs['python']
        )
        has_grc = False
        if self.info['lang'] == 'cpp':
            self._run_lib()
            has_grc = has_pybind
        else:  # Python
            self._run_python()
            if self.info['blocktype'] != 'noblock':
                has_grc = True
        if has_pybind:
            self._run_pybind()
        if self.add_py_qa:
            self._run_python_qa()
        if has_grc and not self.skip_subdirs['grc']:
            self._run_grc()

    def _run_cc_qa(self):
        " Add C++ QA files for 3.7 API if intructed from _run_lib"
        blockname_ = self.info['blockname']
        fname_qa_h = f'qa_{blockname_}.h'
        fname_qa_cc = f'qa_{blockname_}.cc'
        self._write_tpl('qa_cpp', 'lib', fname_qa_cc)
        self._write_tpl('qa_h', 'lib', fname_qa_h)
        modname_ = self.info['modname']
        if self.skip_cmakefiles:
            return
        try:
            append_re_line_sequence(self._file['cmlib'],
                                    fr'list\(APPEND test_{modname_}_sources.*\n',
                                    f'qa_{blockname_}.cc')
            append_re_line_sequence(self._file['qalib'],
                                    '#include.*\n',
                                    f'#include "{fname_qa_h}"')
            append_re_line_sequence(self._file['qalib'],
                                    '(addTest.*suite.*\n|new CppUnit.*TestSuite.*\n)',
                                    f'  s->addTest(gr::{modname_}::qa_{blockname_}::suite());'
                                    )
            self.scm.mark_files_updated((self._file['qalib'],))
        except IOError:
            logger.warning("Can't add C++ QA files.")

    def _run_cc_qa_boostutf(self):
        " Add C++ QA files for 3.8 API if intructed from _run_lib"
        blockname_ = self.info['blockname']
        fname_qa_cc = f'qa_{blockname_}.cc'
        self._write_tpl('qa_cpp_boostutf', 'lib', fname_qa_cc)
        modname_ = self.info['modname']
        if self.skip_cmakefiles:
            return
        try:
            append_re_line_sequence(self._file['cmlib'],
                                    fr'list\(APPEND test_{modname_}_sources.*\n',
                                    f'qa_{blockname_}.cc')
            self.scm.mark_files_updated((self._file['cmlib'],))
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
        if self.info['version'] in ('38', '310'):
            fname_h = self.info['blockname'] + '.h'
            fname_cc = self.info['blockname'] + '.cc'
            if self.info['blocktype'] in ('source', 'sink', 'sync', 'decimator',
                                          'interpolator', 'general', 'hier', 'tagged_stream'):
                fname_cc = self.info['blockname'] + '_impl.cc'
                self._write_tpl('block_impl_h', 'lib',
                                self.info['blockname'] + '_impl.h')
            self._write_tpl('block_impl_cpp', 'lib', fname_cc)
            self._write_tpl('block_def_h', self.info['includedir'], fname_h)
        else:  # Pre-3.8 or autotools
            raise RuntimeError(f"Unsupported version {self.info['version']}")
        if self.add_cc_qa:
            self._run_cc_qa_boostutf()
        if not self.skip_cmakefiles:
            ed = CMakeFileEditor(self._file['cmlib'])
            cmake_list_var = '[a-z]*_?' + self.info['modname'] + '_sources'
            if not ed.append_value('list', fname_cc, to_ignore_start='APPEND ' + cmake_list_var):
                ed.append_value('add_library', fname_cc)
            ed.write()
            ed = CMakeFileEditor(self._file['cminclude'])
            ed.append_value('install', fname_h,
                            to_ignore_end='DESTINATION[^()]+')
            ed.write()
            self.scm.mark_files_updated(
                (self._file['cminclude'], self._file['cmlib']))

    def _run_pybind(self):
        """ Do everything that needs doing in the python bindings subdir.
        - add blockname_python.cc
        - add reference and call to bind_blockname()
        - include them into CMakeLists.txt
        """
        import hashlib

        bindings_dir = os.path.join(self.info['pydir'], 'bindings')

        # Generate bindings cc file
        fname_cc = self.info['blockname'] + '_python.cc'
        fname_pydoc_h = os.path.join(
            'docstrings', self.info['blockname'] + '_pydoc_template.h')

        # Update python_bindings.cc
        ed = CPPFileEditor(self._file['ccpybind'])
        ed.append_value('// BINDING_FUNCTION_PROTOTYPES(', '// ) END BINDING_FUNCTION_PROTOTYPES',
                        'void bind_' + self.info['blockname'] + '(py::module& m);')
        ed.append_value('// BINDING_FUNCTION_CALLS(', '// ) END BINDING_FUNCTION_CALLS',
                        'bind_' + self.info['blockname'] + '(m);')
        ed.write()

        self.scm.mark_files_updated((self._file['ccpybind'],))

        if self.info['version'] in ['310']:
            prefix_include_root = '/'.join(('gnuradio', self.info['modname']))
        else:
            prefix_include_root = self.info['modname']
        blktype = self.info['blocktype']

        bg = BindingGenerator(prefix=gr.prefix(), namespace=[
                              'gr', self.info['modname']], prefix_include_root=prefix_include_root,
                              flag_automatic=(blktype != 'noblock'))
        block_base = code_generator.GRTYPELIST.get(blktype, '')

        header_file = self.info['blockname'] + '.h'
        hasher = hashlib.md5()
        with open(os.path.join(self.info['includedir'], header_file), 'rb') as file_in:
            buf = file_in.read()
            hasher.update(buf)
        md5hash = hasher.hexdigest()

        header_info = {
            "module_name": self.info['modname'],
            "filename": header_file,
            "md5hash": md5hash,
            "namespace": {
                "name": "::".join(['gr', self.info['modname']]),
                "enums": [],
                "variables": [],
                "classes": [
                    {
                        "name": self.info['blockname'],
                        "constructors": [
                            {
                                "name": self.info['blockname'],
                                "arguments": []
                            }
                        ]
                    }
                ],
                "free_functions": [],
                "namespaces": []
            }
        }

        if block_base:
            header_info['namespace']['classes'][0]['bases'] = ["::", "gr", block_base]

        if blktype != 'noblock':
            # Only blocks have make
            header_info['namespace']['classes'][0]['member_functions'] = [
                {
                    "name": "make",
                    "return_type": f"gr::{self.info['modname']}::{self.info['blockname']}::sptr",
                    "has_static": "1",
                    "arguments": []
                }
            ]

        # def gen_pybind_cc(self, header_info, base_name):
        pydoc_txt = bg.gen_pydoc_h(header_info, self.info['blockname'])
        path_to_file = os.path.join(bindings_dir, fname_pydoc_h)
        logger.info("Adding file '{}'...".format(path_to_file))
        with open(path_to_file, 'w') as f:
            f.write(pydoc_txt)
        self.scm.add_files((path_to_file,))

        cc_txt = bg.gen_pybind_cc(header_info, self.info['blockname'])
        path_to_file = os.path.join(bindings_dir, fname_cc)
        logger.info("Adding file '{}'...".format(path_to_file))
        with open(path_to_file, 'w') as f:
            f.write(cc_txt)
        self.scm.add_files((path_to_file,))

        if not self.skip_cmakefiles:
            ed = CMakeFileEditor(self._file['cmpybind'])
            cmake_list_var = 'APPEND {}_python_files'.format(
                self.info['modname'])
            ed.append_value('list', fname_cc, to_ignore_start=cmake_list_var,
                            to_ignore_end='python_bindings.cc')
            ed.write()
            self.scm.mark_files_updated((self._file['cmpybind'],))

    def _run_python_qa(self):
        """ Do everything that needs doing in the subdir 'python' to add
        QA code.
        - add .py files
        - include in CMakeLists.txt
        """
        fname_py_qa = 'qa_' + self.info['blockname'] + '.py'
        self._write_tpl('qa_python', self.info['pydir'], fname_py_qa)
        os.chmod(os.path.join(self.info['pydir'], fname_py_qa), 0o755)
        self.scm.mark_files_updated(
            (os.path.join(self.info['pydir'], fname_py_qa),))
        if self.skip_cmakefiles or CMakeFileEditor(self._file['cmpython']).check_for_glob('qa_*.py'):
            return
        logger.info(f'Editing {self.info["pydir"]}/CMakeLists.txt...')
        with open(self._file['cmpython'], 'a') as f:
            f.write(
                'GR_ADD_TEST(qa_%s ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/%s)\n' %
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
        blockname_ = self.info['blockname']
        self._write_tpl('block_python', self.info['pydir'], fname_py)
        append_re_line_sequence(self._file['pyinit'],
                                '(^from.*import.*\n|# import any pure.*\n)',
                                f'from .{blockname_} import {blockname_}')
        self.scm.mark_files_updated((self._file['pyinit'],))
        if self.skip_cmakefiles:
            return
        ed = CMakeFileEditor(self._file['cmpython'])
        ed.append_value('GR_PYTHON_INSTALL', fname_py,
                        to_ignore_end='DESTINATION[^()]+')
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
        ed.append_value('install', fname_grc,
                        to_ignore_end='DESTINATION[^()]+')
        ed.write()
        self.scm.mark_files_updated((self._file['cmgrc'],))
