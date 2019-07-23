"""
Copyright 2016 Free Software Foundation, Inc.
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

from __future__ import absolute_import, print_function

import sys
import os

from ..core.Config import Config as CoreConfig
from . import Constants

from six.moves import configparser

HEADER = """\
# This contains only GUI settings for GRC and is not meant for users to edit.
#
# GRC settings not accessible through the GUI are in gnuradio.conf under
# section [grc].

"""


class Config(CoreConfig):

    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get(
        'GRC_PREFS_PATH', os.path.expanduser('~/.gnuradio/grc.conf'))

    def __init__(self, install_prefix, *args, **kwargs):
        CoreConfig.__init__(self, *args, **kwargs)
        self.install_prefix = install_prefix
        Constants.update_font_size(self.font_size)

        self.parser = configparser.ConfigParser()
        for section in ['main', 'files_open', 'files_recent']:
            try:
                self.parser.add_section(section)
            except Exception as e:
                print(e)
        try:
            self.parser.read(self.gui_prefs_file)
        except Exception as err:
            print(err, file=sys.stderr)

    def save(self):
        try:
            with open(self.gui_prefs_file, 'w') as fp:
                fp.write(HEADER)
                self.parser.write(fp)
        except Exception as err:
            print(err, file=sys.stderr)

    def entry(self, key, value=None, default=None):
        if value is not None:
            self.parser.set('main', key, str(value))
            result = value
        else:
            _type = type(default) if default is not None else str
            getter = {
                bool: self.parser.getboolean,
                int: self.parser.getint,
            }.get(_type, self.parser.get)
            try:
                result = getter('main', key)
            except (AttributeError, configparser.Error):
                result = _type() if default is None else default
        return result

    @property
    def editor(self):
        return self._gr_prefs.get_string('grc', 'editor', '')

    @editor.setter
    def editor(self, value):
        self._gr_prefs.get_string('grc', 'editor', value)
        self._gr_prefs.save()

    @property
    def xterm_executable(self):
        return self._gr_prefs.get_string('grc', 'xterm_executable', 'xterm')

    @property
    def wiki_block_docs_url_prefix(self):
        return self._gr_prefs.get_string('grc-docs', 'wiki_block_docs_url_prefix', '')

    @property
    def default_canvas_size(self):
        try:  # ugly, but matches current code style
            raw = self._gr_prefs.get_string('grc', 'canvas_default_size', '1280, 1024')
            value = tuple(int(x.strip('() ')) for x in raw.split(','))
            if len(value) != 2 or not all(300 < x < 4096 for x in value):
                raise ValueError
            return value
        except (ValueError, TypeError):
            print("Error: invalid 'canvas_default_size' setting.", file=sys.stderr)
            return Constants.DEFAULT_CANVAS_SIZE_DEFAULT

    @property
    def font_size(self):
        try:  # ugly, but matches current code style
            font_size = self._gr_prefs.get_long('grc', 'canvas_font_size',
                                                Constants.DEFAULT_FONT_SIZE)
            if font_size <= 0:
                raise ValueError
        except (ValueError, TypeError):
            font_size = Constants.DEFAULT_FONT_SIZE
            print("Error: invalid 'canvas_font_size' setting.", file=sys.stderr)

        return font_size

    @property
    def default_qss_theme(self):
        return self._gr_prefs.get_string('qtgui', 'qss', '')

    @default_qss_theme.setter
    def default_qss_theme(self, value):
        self._gr_prefs.set_string("qtgui", "qss", value)
        self._gr_prefs.save()

    ##### Originally from Preferences.py #####
    def main_window_size(self, size=None):
        if size is None:
            size = [None, None]
        w = self.entry('main_window_width', size[0], default=800)
        h = self.entry('main_window_height', size[1], default=600)
        return w, h

    def file_open(self, filename=None):
        return self.entry('file_open', filename, default='')

    def set_file_list(self, key, files):
        self.parser.remove_section(key)  # clear section
        self.parser.add_section(key)
        for i, filename in enumerate(files):
            self.parser.set(key, '%s_%d' % (key, i), filename)

    def get_file_list(self, key):
        try:
            files = [value for name, value in self.parser.items(key)
                     if name.startswith('%s_' % key)]
        except (AttributeError, configparser.Error):
            files = []
        return files

    def get_open_files(self):
        return self.get_file_list('files_open')

    def set_open_files(self, files):
        return self.set_file_list('files_open', files)

    def get_recent_files(self):
        """ Gets recent files, removes any that do not exist and re-saves it """
        files = list(filter(os.path.exists, self.get_file_list('files_recent')))
        self.set_recent_files(files)
        return files

    def set_recent_files(self, files):
        return self.set_file_list('files_recent', files)

    def add_recent_file(self, file_name):
        # double check file_name
        if os.path.exists(file_name):
            recent_files = self.get_recent_files()
            if file_name in recent_files:
                recent_files.remove(file_name)  # Attempt removal
            recent_files.insert(0, file_name)  # Insert at start
            self.set_recent_files(recent_files[:10])  # Keep up to 10 files

    def console_window_position(self, pos=None):
        return self.entry('console_window_position', pos, default=-1) or 1

    def blocks_window_position(self, pos=None):
        return self.entry('blocks_window_position', pos, default=-1) or 1

    def variable_editor_position(self, pos=None, sidebar=False):
        # Figure out default
        if sidebar:
            w, h = self.main_window_size()
            return self.entry('variable_editor_sidebar_position', pos, default=int(h*0.7))
        else:
            return self.entry('variable_editor_position', pos, default=int(self.blocks_window_position()*0.5))

    def variable_editor_sidebar(self, pos=None):
        return self.entry('variable_editor_sidebar', pos, default=False)

    def variable_editor_confirm_delete(self, pos=None):
        return self.entry('variable_editor_confirm_delete', pos, default=True)

    def xterm_missing(self, cmd=None):
        return self.entry('xterm_missing', cmd, default='INVALID_XTERM_SETTING')

    def screen_shot_background_transparent(self, transparent=None):
        return self.entry('screen_shot_background_transparent', transparent, default=False)
