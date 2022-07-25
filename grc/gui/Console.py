"""
Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from ..core import Messages
from .Dialogs import TextDisplay, MessageDialogWrapper
from .Constants import DEFAULT_CONSOLE_WINDOW_WIDTH
from gi.repository import Gtk, Gdk, GObject
import os
import logging

import gi
gi.require_version('Gtk', '3.0')


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
