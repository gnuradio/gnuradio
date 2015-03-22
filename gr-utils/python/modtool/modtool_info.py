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
""" Returns information about a module """

import os
from optparse import OptionGroup

from modtool_base import ModTool, ModToolException
from util_functions import get_modname


class ModToolInfo(ModTool):
    """ Return information about a given module """
    name = 'info'
    aliases = ('getinfo', 'inf')

    def __init__(self):
        ModTool.__init__(self)
        self._directory = None
        self._python_readable = False
        self._suggested_dirs = None

    def setup_parser(self):
        """ Initialise the option parser for 'gr_modtool info' """
        parser = ModTool.setup_parser(self)
        parser.usage = '%prog info [options]. \n Call %prog without any options to run it interactively.'
        ogroup = OptionGroup(parser, "Info options")
        ogroup.add_option("--python-readable", action="store_true", default=None,
                help="Return the output in a format that's easier to read for Python scripts.")
        ogroup.add_option("--suggested-dirs", default=None, type="string",
                help="Suggest typical include dirs if nothing better can be detected.")
        parser.add_option_group(ogroup)
        return parser

    def setup(self, options, args):
        # Won't call parent's setup(), because that's too chatty
        self._directory = options.directory
        self._python_readable = options.python_readable
        self._suggested_dirs = options.suggested_dirs

    def run(self):
        """ Go, go, go! """
        mod_info = dict()
        mod_info['base_dir'] = self._get_base_dir(self._directory)
        if mod_info['base_dir'] is None:
            raise ModToolException('{}' if self._python_readable else "No module found.")
        os.chdir(mod_info['base_dir'])
        mod_info['modname'] = get_modname()
        if mod_info['modname'] is None:
            raise ModToolException('{}' if self._python_readable else "No module found.")
        if self._info['version'] == '36' and (
                os.path.isdir(os.path.join('include', mod_info['modname'])) or
                os.path.isdir(os.path.join('include', 'gnuradio', mod_info['modname']))
                ):
            self._info['version'] = '37'
        mod_info['version'] = self._info['version']
        if 'is_component' in self._info.keys() and self._info['is_component']:
            mod_info['is_component'] = True
        mod_info['incdirs'] = []
        mod_incl_dir = os.path.join(mod_info['base_dir'], 'include')
        if os.path.isdir(os.path.join(mod_incl_dir, mod_info['modname'])):
            mod_info['incdirs'].append(os.path.join(mod_incl_dir, mod_info['modname']))
        else:
            mod_info['incdirs'].append(mod_incl_dir)
        build_dir = self._get_build_dir(mod_info)
        if build_dir is not None:
            mod_info['build_dir'] = build_dir
            mod_info['incdirs'] += self._get_include_dirs(mod_info)
        if self._python_readable:
            print str(mod_info)
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
        if 'is_component' in mod_info.keys():
            (base_build_dir, rest_dir) = os.path.split(base_build_dir)
        has_build_dir = os.path.isdir(os.path.join(base_build_dir , 'build'))
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
                            False: 'PATH'}['is_component' in mod_info.keys()]
        try:
            cmakecache_fid = open(os.path.join(mod_info['build_dir'], 'CMakeCache.txt'))
            for line in cmakecache_fid:
                if line.find('GNURADIO_RUNTIME_INCLUDE_DIRS:%s' % path_or_internal) != -1:
                    inc_dirs += line.replace('GNURADIO_RUNTIME_INCLUDE_DIRS:%s=' % path_or_internal, '').strip().split(';')
        except IOError:
            pass
        if len(inc_dirs) == 0 and self._suggested_dirs is not None:
            inc_dirs = [os.path.normpath(path) for path in self._suggested_dirs.split(':') if os.path.isdir(path)]
        return inc_dirs

    def _pretty_print(self, mod_info):
        """ Output the module info in human-readable format """
        index_names = {'base_dir': 'Base directory',
                       'modname':  'Module name',
                       'is_component':  'Is GR component',
                       'build_dir': 'Build directory',
                       'incdirs': 'Include directories'}
        for key in mod_info.keys():
            if key == 'version':
                print "        API version: %s" % {
                        '36': 'pre-3.7',
                        '37': 'post-3.7',
                        'autofoo': 'Autotools (pre-3.5)'
                        }[mod_info['version']]
            else:
                print '%19s: %s' % (index_names[key], mod_info[key])
