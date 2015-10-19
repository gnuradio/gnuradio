"""
Copyright 2008 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import sys
import ConfigParser


HEADER = """\
# This contains only GUI settings for GRC and is not meant for users to edit.
#
# GRC settings not accessible through the GUI are in gnuradio.conf under
# section [grc].

"""

_platform = None
_config_parser = ConfigParser.SafeConfigParser()


def file_extension():
    return '.'+_platform.get_key()


def load(platform):
    global _platform
    _platform = platform
    # create sections
    _config_parser.add_section('main')
    _config_parser.add_section('files_open')
    try:
        _config_parser.read(_platform.get_prefs_file())
    except Exception as err:
        print >> sys.stderr, err


def save():
    try:
        with open(_platform.get_prefs_file(), 'w') as fp:
            fp.write(HEADER)
            _config_parser.write(fp)
    except Exception as err:
        print >> sys.stderr, err


def entry(key, value=None, default=None):
    if value is not None:
        _config_parser.set('main', key, str(value))
        result = value
    else:
        _type = type(default) if default is not None else str
        getter = {
            bool: _config_parser.getboolean,
            int: _config_parser.getint,
        }.get(_type, _config_parser.get)
        try:
            result = getter('main', key)
        except ConfigParser.Error:
            result = _type() if default is None else default
    return result


###########################################################################
# Special methods for specific program functionalities
###########################################################################

def main_window_size(size=None):
    if size is None:
        size = [None, None]
    w = entry('main_window_width', size[0], default=1)
    h = entry('main_window_height', size[1], default=1)
    return w, h


def file_open(filename=None):
    return entry('file_open', filename, default='')


def files_open(files=None):
    if files is not None:
        _config_parser.remove_section('files_open')  # clear section
        _config_parser.add_section('files_open')
        for i, filename in enumerate(files):
            _config_parser.set('files_open', 'file_open_%d' % i, filename)

    else:
        try:
            files = [value for name, value in _config_parser.items('files_open')
                     if name.startswith('file_open_')]
        except ConfigParser.Error:
            files = []
        return files


def reports_window_position(pos=None):
    return entry('reports_window_position', pos, default=-1) or 1


def blocks_window_position(pos=None):
    return entry('blocks_window_position', pos, default=-1) or 1


def xterm_missing(cmd=None):
    return entry('xterm_missing', cmd, default='INVALID_XTERM_SETTING')
