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

import os
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
    return '.grc'


def load(platform):
    global _platform
    _platform = platform
    # create sections
    for section in ['main', 'files_open', 'files_recent']:
        try:
            _config_parser.add_section(section)
        except Exception, e:
             print e
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
        except (AttributeError, ConfigParser.Error):
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


def set_file_list(key, files):
    _config_parser.remove_section(key)  # clear section
    _config_parser.add_section(key)
    for i, filename in enumerate(files):
        _config_parser.set(key, '%s_%d' % (key, i), filename)


def get_file_list(key):
    try:
        files = [value for name, value in _config_parser.items(key)
                 if name.startswith('%s_' % key)]
    except (AttributeError, ConfigParser.Error):
        files = []
    return files


def get_open_files():
    return get_file_list('files_open')


def set_open_files(files):
    return set_file_list('files_open', files)


def get_recent_files():
    """ Gets recent files, removes any that do not exist and re-saves it """
    files = filter(os.path.exists, get_file_list('files_recent'))
    set_recent_files(files)
    return files


def set_recent_files(files):
    return set_file_list('files_recent', files)


def add_recent_file(file_name):
    # double check file_name
    if os.path.exists(file_name):
        recent_files = get_recent_files()
        if file_name in recent_files:
            recent_files.remove(file_name)  # Attempt removal
        recent_files.insert(0, file_name)  # Insert at start
        set_recent_files(recent_files[:10])  # Keep up to 10 files


def console_window_position(pos=None):
    return entry('console_window_position', pos, default=-1) or 1


def blocks_window_position(pos=None):
    return entry('blocks_window_position', pos, default=-1) or 1


def variable_editor_position(pos=None, sidebar=False):
    # Figure out default
    if sidebar:
        w, h = main_window_size()
        return entry('variable_editor_sidebar_position', pos, default=int(h*0.7))
    else:
        return entry('variable_editor_position', pos, default=int(blocks_window_position()*0.5))


def variable_editor_sidebar(pos=None):
    return entry('variable_editor_sidebar', pos, default=False)


def variable_editor_confirm_delete(pos=None):
    return entry('variable_editor_confirm_delete', pos, default=True)


def xterm_missing(cmd=None):
    return entry('xterm_missing', cmd, default='INVALID_XTERM_SETTING')


def screen_shot_background_transparent(transparent=None):
    return entry('screen_shot_background_transparent', transparent, default=False)
