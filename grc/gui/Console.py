"""
Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
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

from __future__ import absolute_import

import os
import logging

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GObject

from .Constants import DEFAULT_CONSOLE_WINDOW_WIDTH
from .Dialogs import TextDisplay, MessageDialogWrapper

from ..core import Messages


log = logging.getLogger(__name__)


class Console(Gtk.ScrolledWindow):
    def __init__(self):
        Gtk.ScrolledWindow.__init__(self)
        log.debug("console()")
        self.app = Gtk.Application.get_default()

        self.text_display = TextDisplay()

        self.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        self.add(self.text_display)
        self.set_size_request(-1, DEFAULT_CONSOLE_WINDOW_WIDTH)

    def add_line(self, line):
        """
        Place line at the end of the text buffer, then scroll its window all the way down.

        Args:
            line: the new text
        """
        self.text_display.insert(line)
