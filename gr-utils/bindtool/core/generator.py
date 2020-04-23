#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from .base import BindTool
from gnuradio.blocktool import BlockHeaderParser, GenericHeaderParser

import os
import pathlib
import json
from mako.template import Template
from datetime import datetime


class BindingGenerator:

    def __init__(self, prefix, namespace, prefix_include_root, output_dir="", addl_includes="", match_include_structure=False):
        """Initialize BindingGenerator
        prefix -- path to installed gnuradio prefix (use gr.prefix() if unsure)
        namespace -- desired namespace to parse e.g. ['gr','module_name']
            module_name is stored as the last element of namespace
        prefix_include_root -- relative path to module headers, e.g. "gnuradio/modulename"

        Keyword arguments:
        output_dir -- path where bindings will be placed
        addl_includes -- comma separated list of additional include directories (default "")
        match_include_structure -- 
            If set to False, a bindings/ dir will be placed directly under the specified output_dir
            If set to True, the directory structure under include/ will be mirrored
        """

        self.header_extensions = ['.h', '.hh', '.hpp']
        self.addl_include = addl_includes
        self.prefix = prefix
        self.namespace = namespace
        self.module_name = namespace[-1]
        self.prefix_include_root = prefix_include_root
        self.output_dir = output_dir
        self.match_include_structure = match_include_structure

        pass

    def gen_pydoc_h(self, header_info, base_name):
        current_path = os.path.dirname(pathlib.Path(__file__).absolute())
        tpl = Template(filename=os.path.join(
            current_path, '..', 'templates', 'license.mako'))
        license = tpl.render(year=datetime.now().year)

        tpl = Template(filename=os.path.join(current_path, '..',
                                             'templates', 'pydoc_h.mako'))
        return tpl.render(
            license=license,
            header_info=header_info,
            basename=base_name,
            prefix_include_root=self.prefix_include_root,
        )


    def gen_pybind_cc(self, header_info, base_name):
        current_path = os.path.dirname(pathlib.Path(__file__).absolute())
        tpl = Template(filename=os.path.join(
            current_path, '..', 'templates', 'license.mako'))
        license = tpl.render(year=datetime.now().year)

        tpl = Template(filename=os.path.join(current_path, '..',
                                             'templates', 'generic_python_cc.mako'))
        return tpl.render(
            license=license,
            header_info=header_info,
            basename=base_name,
            prefix_include_root=self.prefix_include_root,
        )

    def write_pydoc_h(self, header_info, base_name, output_dir):

        doc_pathname = os.path.join(
            output_dir, 'docstrings', '{}_pydoc_template.h'.format(base_name))

        try:
            pybind_code = self.gen_pydoc_h(
                header_info, base_name)
            with open(doc_pathname, 'w+') as outfile:
                print("Writing binding code to {}".format(doc_pathname))
                outfile.write(pybind_code)
            return doc_pathname
        except Exception as e:
            print(e)
            return None


    def write_pybind_cc(self, header_info, base_name, output_dir):

        binding_pathname_cc = os.path.join(
            output_dir, '{}_python.cc'.format(base_name))

        try:
            pybind_code = self.gen_pybind_cc(
                header_info, base_name)
            with open(binding_pathname_cc, 'w+') as outfile:
                print("Writing binding code to {}".format(binding_pathname_cc))
                outfile.write(pybind_code)
            return binding_pathname_cc
        except Exception as e:
            print(e)
            return None

    def write_json(self, header_info, base_name, output_dir):
        json_pathname = os.path.join(output_dir, '{}.json'.format(base_name))
        with open(json_pathname, 'w') as outfile:
            json.dump(header_info, outfile)

    def read_json(self, pathname):
        with open(pathname, 'r') as fp:
            header_info = json.load(fp)
        return header_info

    def gen_file_binding(self, file_to_process):
        """Produce the blockname_python.cc python bindings"""
        output_dir = self.get_output_dir(file_to_process)
        binding_pathname = None
        base_name = os.path.splitext(os.path.basename(file_to_process))[0]
        module_include_path = os.path.abspath(os.path.dirname(file_to_process))
        top_include_path = os.path.join(
            module_include_path.split('include'+os.path.sep)[0], 'include')

        include_paths = ','.join(
            (module_include_path, top_include_path))
        if self.prefix:
            prefix_include_path = os.path.abspath(
                os.path.join(self.prefix, 'include'))
            include_paths = ','.join(
                (include_paths, prefix_include_path)
            )
        if self.addl_include:
            include_paths = ','.join((include_paths, self.addl_include))

        parser = GenericHeaderParser(
            include_paths=include_paths, file_path=file_to_process)
        try:
            header_info = parser.get_header_info(self.namespace)
            # TODO: Scrape the docstrings
            self.write_json(header_info, base_name, output_dir)
            self.write_pybind_cc(header_info, base_name, output_dir)
            self.write_pydoc_h(header_info, base_name, output_dir)

        except Exception as e:
            print(e)
            failure_pathname = os.path.join(
                output_dir, 'failed_conversions.txt')
            with open(failure_pathname, 'a+') as outfile:
                outfile.write(file_to_process)
                outfile.write(str(e))
                outfile.write('\n')

        return binding_pathname

    def get_output_dir(self, filename):
        """Get the output directory for a given file"""
        output_dir = self.output_dir
        rel_path_after_include = ""
        if self.match_include_structure:
            if 'include'+os.path.sep in filename:
                rel_path_after_include = os.path.split(
                    filename.split('include'+os.path.sep)[-1])[0]

        output_dir = os.path.join(
            self.output_dir, rel_path_after_include, 'bindings')
        doc_dir = os.path.join(output_dir,'docstrings')
        if output_dir and not os.path.exists(output_dir) and not os.path.exists(doc_dir):
            output_dir = os.path.abspath(output_dir)
            print('creating output directory {}'.format(output_dir))
            os.makedirs(output_dir)

        if doc_dir and not os.path.exists(doc_dir):
            doc_dir = os.path.abspath(doc_dir)
            print('creating docstrings directory {}'.format(doc_dir))
            os.makedirs(doc_dir)

        return output_dir

    def gen_top_level_cpp(self, file_list):
        """Produce the python_bindings.cc for the bindings"""
        current_path = os.path.dirname(pathlib.Path(__file__).absolute())
        file = file_list[0]
        output_dir = self.get_output_dir(file)

        tpl = Template(filename=os.path.join(
            current_path, '..', 'templates', 'license.mako'))
        license = tpl.render(year=datetime.now().year)

        binding_pathname = os.path.join(output_dir, 'python_bindings.cc')
        file_list = [os.path.split(f)[-1] for f in file_list]
        tpl = Template(filename=os.path.join(current_path, '..',
                                             'templates', 'python_bindings_cc.mako'))
        pybind_code = tpl.render(
            license=license,
            files=file_list,
            module_name=self.module_name
        )

        # print(pybind_code)
        try:
            with open(binding_pathname, 'w+') as outfile:
                outfile.write(pybind_code)
            return binding_pathname
        except:
            return None

    def gen_cmake_lists(self, file_list):
        """Produce the CMakeLists.txt for the bindings"""
        current_path = os.path.dirname(pathlib.Path(__file__).absolute())
        file = file_list[0]
        output_dir = self.get_output_dir(file)

        tpl = Template(filename=os.path.join(
            current_path, '..', 'templates', 'license.mako'))
        license = tpl.render(year=datetime.now().year)

        binding_pathname = os.path.join(output_dir, 'CMakeLists.txt')
        file_list = [os.path.split(f)[-1] for f in file_list]
        tpl = Template(filename=os.path.join(current_path, '..',
                                             'templates', 'CMakeLists.txt.mako'))
        pybind_code = tpl.render(
            license=license,
            files=file_list,
            module_name=self.module_name
        )

        # print(pybind_code)
        try:
            with open(binding_pathname, 'w+') as outfile:
                outfile.write(pybind_code)
            return binding_pathname
        except:
            return None

    def get_file_list(self, include_path):
        """Recursively get sorted list of files in path"""
        file_list = []
        for root, _, files in os.walk(include_path):
            for file in files:
                _, file_extension = os.path.splitext(file)
                if (file_extension in self.header_extensions):
                    pathname = os.path.abspath(os.path.join(root, file))
                    file_list.append(pathname)
        return sorted(file_list)

    def gen_bindings(self, module_dir):
        """Generate bindings for an entire GR module

        Produces CMakeLists.txt, python_bindings.cc, and blockname_python.cc 
            for each block in the module

        module_dir -- path to the include directory where the public headers live
        """
        file_list = self.get_file_list(module_dir)
        api_pathnames = [s for s in file_list if 'api.h' in s]
        for f in api_pathnames:
            file_list.remove(f)
        self.gen_top_level_cpp(file_list)
        self.gen_cmake_lists(file_list)
        for fn in file_list:
            self.gen_file_binding(fn)
