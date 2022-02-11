#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Returns information about a module """


import os

from ..tools import get_modname
from .base import ModTool, ModToolException


class ModToolInfo(ModTool):
    """ Return information about a given module """
    name = 'info'
    description = 'Return information about a given module.'

    def __init__(self, python_readable=False, suggested_dirs=None, **kwargs):
        ModTool.__init__(self, **kwargs)
        # Don't call ModTool._validate(), is is too chatty!
        self._directory = self.dir
        self._python_readable = python_readable
        self._suggested_dirs = suggested_dirs

    def run(self):
        """ Go, go, go! """
        mod_info = dict()
        mod_info['base_dir'] = self._get_base_dir(self._directory)
        if mod_info['base_dir'] is None:
            raise ModToolException(
                '{}' if self._python_readable else "No module found.")
        os.chdir(mod_info['base_dir'])
        mod_info['modname'] = get_modname()
        if mod_info['modname'] is None:
            raise ModToolException(
                '{}' if self._python_readable else "No module found.")
        if self.info['version'] == '36' and (
                os.path.isdir(os.path.join('include', mod_info['modname'])) or
                os.path.isdir(os.path.join(
                    'include', 'gnuradio', mod_info['modname']))
        ):
            self.info['version'] = '37'
        if not os.path.isfile(os.path.join('cmake', 'Modules', 'FindCppUnit.cmake')):
            self.info['version'] = '38'
            if os.path.isdir(os.path.join('include', 'gnuradio', mod_info['modname'])):
                self.info['version'] = '310'
        mod_info['version'] = self.info['version']
        if 'is_component' in list(self.info.keys()) and self.info['is_component']:
            mod_info['is_component'] = True
        mod_info['incdirs'] = []
        mod_incl_dir = os.path.join(mod_info['base_dir'], 'include')
        if os.path.isdir(os.path.join(mod_incl_dir, mod_info['modname'])):
            mod_info['incdirs'].append(os.path.join(
                mod_incl_dir, mod_info['modname']))
        else:
            mod_info['incdirs'].append(mod_incl_dir)
        build_dir = self._get_build_dir(mod_info)
        if build_dir is not None:
            mod_info['build_dir'] = build_dir
            mod_info['incdirs'] += self._get_include_dirs(mod_info)
        if self._python_readable:
            print(str(mod_info))
        else:
            self._pretty_print(mod_info)

    def _get_base_dir(self, start_dir):
        """ Figure out the base dir (where the top-level cmake file is) """
        base_dir = os.path.abspath(start_dir)
        if self._check_directory(base_dir):
            return base_dir
        else:
            (up_dir, this_dir) = os.path.split(base_dir)
            if os.path.split(up_dir)[1] == 'include':
                up_dir = os.path.split(up_dir)[0]
            if self._check_directory(up_dir):
                return up_dir
        return None

    def _get_build_dir(self, mod_info):
        """ Figure out the build dir (i.e. where you run 'cmake'). This checks
        for a file called CMakeCache.txt, which is created when running cmake.
        If that hasn't happened, the build dir cannot be detected, unless it's
        called 'build', which is then assumed to be the build dir. """
        base_build_dir = mod_info['base_dir']
        if 'is_component' in list(mod_info.keys()):
            (base_build_dir, rest_dir) = os.path.split(base_build_dir)
        has_build_dir = os.path.isdir(os.path.join(base_build_dir, 'build'))
        if (has_build_dir and os.path.isfile(os.path.join(base_build_dir, 'CMakeCache.txt'))):
            return os.path.join(base_build_dir, 'build')
        else:
            for (dirpath, dirnames, filenames) in os.walk(base_build_dir):
                if 'CMakeCache.txt' in filenames:
                    return dirpath
        if has_build_dir:
            return os.path.join(base_build_dir, 'build')
        return None

    def _get_include_dirs(self, mod_info):
        """ Figure out include dirs for the make process. """
        inc_dirs = []
        path_or_internal = {True: 'INTERNAL',
                            False: 'PATH'}['is_component' in list(mod_info.keys())]
        try:
            cmakecache_fid = open(os.path.join(
                mod_info['build_dir'], 'CMakeCache.txt'))
            for line in cmakecache_fid:
                if line.find(f'GNURADIO_RUNTIME_INCLUDE_DIRS:{path_or_internal}') != -1:
                    inc_dirs += line.replace(
                        f'GNURADIO_RUNTIME_INCLUDE_DIRS:{path_or_internal}=', '').strip().split(';')
        except IOError:
            pass
        if not inc_dirs and self._suggested_dirs is not None:
            inc_dirs = [os.path.normpath(path) for path in self._suggested_dirs.split(
                ':') if os.path.isdir(path)]
        return inc_dirs

    def _pretty_print(elf, mod_info):
        """ Output the module info in human-readable format """
        index_names = {'base_dir': 'Base directory',
                       'modname': 'Module name',
                       'is_component': 'Is GR component',
                       'build_dir': 'Build directory',
                       'incdirs': 'Include directories'}
        for key in list(mod_info.keys()):
            if key == 'version':
                version = {
                    '36': 'pre-3.7',
                    '37': 'post-3.7',
                    '38': 'post-3.8',
                    '310': 'post-3.10',
                    'autofoo': 'Autotools (pre-3.5)'
                }[mod_info['version']]
                print(f"        API version: {version}")
            else:
                print('%19s: %s' % (index_names[key], mod_info[key]))
