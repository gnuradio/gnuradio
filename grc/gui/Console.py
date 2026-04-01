"""
Copyright 2008, 2009, 2011 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from ..core import Messages
from .Dialogs import TextDisplay, MessageDialogWrapper
from .Constants import DEFAULT_CONSOLE_WINDOW_WIDTH
import os
import logging

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GObject, GLib


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

        self._buffered_text = []
        self._flush_id = None

    def _flush_buffer(self):
        if self._buffered_text:
            self.text_display.insert("".join(self._buffered_text))
            self._buffered_text = []
        self._flush_id = None
        return False

    def add_line(self, line):
        """
        Place line at the end of the text buffer, then scroll its window all the way down.

        Args:
            line: the new text
        """
        self._buffered_text.append(line)
        if self._flush_id is None:
            self._flush_id = GLib.timeout_add(100, self._flush_buffer)