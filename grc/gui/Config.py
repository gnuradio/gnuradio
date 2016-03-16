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

import sys
import os
from ..core.Config import Config as _Config
from . import Constants


class Config(_Config):

    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get(
        'GRC_PREFS_PATH', os.path.expanduser('~/.gnuradio/grc.conf'))

    def __init__(self, install_prefix, *args, **kwargs):
        _Config.__init__(self, *args, **kwargs)
        self.install_prefix = install_prefix
        Constants.update_font_size(self.font_size)

    @property
    def editor(self):
        return self.prefs.get_string('grc', 'editor', '')

    @editor.setter
    def editor(self, value):
        self.prefs.get_string('grc', 'editor', value)
        self.prefs.save()

    @property
    def xterm_executable(self):
        return self.prefs.get_string('grc', 'xterm_executable', 'xterm')

    @property
    def default_canvas_size(self):
        try:  # ugly, but matches current code style
            raw = self.prefs.get_string('grc', 'canvas_default_size', '1280, 1024')
            value = tuple(int(x.strip('() ')) for x in raw.split(','))
            if len(value) != 2 or not all(300 < x < 4096 for x in value):
                raise Exception()
            return value
        except:
            print >> sys.stderr, "Error: invalid 'canvas_default_size' setting."
            return Constants.DEFAULT_CANVAS_SIZE_DEFAULT

    @property
    def font_size(self):
        try:  # ugly, but matches current code style
            font_size = self.prefs.get_long('grc', 'canvas_font_size',
                                            Constants.DEFAULT_FONT_SIZE)
            if font_size <= 0:
                raise Exception()
        except:
            font_size = Constants.DEFAULT_FONT_SIZE
            print >> sys.stderr, "Error: invalid 'canvas_font_size' setting."

        return font_size
