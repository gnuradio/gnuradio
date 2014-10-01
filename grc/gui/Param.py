"""
Copyright 2007-2011 Free Software Foundation, Inc.
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

import Utils
from Element import Element
from . Constants import PARAM_FONT
import pygtk
pygtk.require('2.0')
import gtk
import Colors
import os

class InputParam(gtk.HBox):
    """The base class for an input parameter inside the input parameters dialog."""

    def __init__(self, param, callback=None):
        gtk.HBox.__init__(self)
        self.param = param
        self._callback = callback
        self.label = gtk.Label() #no label, markup is added by set_markup
        self.label.set_size_request(150, -1)
        self.pack_start(self.label, False)
        self.set_markup = lambda m: self.label.set_markup(m)
        self.tp = None
        self._changed_but_unchecked = False
        #connect events
        self.connect('show', self._update_gui)
    def set_color(self, color): pass
    def set_tooltip_text(self, text): pass

    def _update_gui(self, *args):
        """
        Set the markup, color, tooltip, show/hide.
        """
        #set the markup
        has_cb = \
            hasattr(self.param.get_parent(), 'get_callbacks') and \
            filter(lambda c: self.param.get_key() in c, self.param.get_parent()._callbacks)
        self.set_markup(Utils.parse_template(PARAM_LABEL_MARKUP_TMPL,
            param=self.param, has_cb=has_cb,
            modified=self._changed_but_unchecked))
        #set the color
        self.set_color(self.param.get_color())
        #set the tooltip
        self.set_tooltip_text(
            Utils.parse_template(TIP_MARKUP_TMPL, param=self.param).strip(),
        )
        #show/hide
        if self.param.get_hide() == 'all': self.hide_all()
        else: self.show_all()

    def _mark_changed(self, *args):
        """
        Mark this param as modified on change, but validate only on focus-lost
        """
        self._changed_but_unchecked = True
        self._update_gui()

    def _apply_change(self, *args):
        """
        Handle a gui change by setting the new param value,
        calling the callback (if applicable), and updating.
        """
        #set the new value
        self.param.set_value(self.get_text())
        #call the callback
        if self._callback: self._callback(*args)
        else: self.param.validate()
        #gui update
        self._changed_but_unchecked = False
        self._update_gui()

class EntryParam(InputParam):
    """Provide an entry box for strings and numbers."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = gtk.Entry()
        self._input.set_text(self.param.get_value())
        self._input.connect('changed', self._mark_changed)
        self._input.connect('focus-out-event', self._apply_change)
        self.pack_start(self._input, True)
    def get_text(self): return self._input.get_text()
    def set_color(self, color):
        self._input.modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))
        self._input.modify_text(gtk.STATE_NORMAL, Colors.PARAM_ENTRY_TEXT_COLOR)
    def set_tooltip_text(self, text): self._input.set_tooltip_text(text)

class EnumParam(InputParam):
    """Provide an entry box for Enum types with a drop down menu."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = gtk.combo_box_new_text()
        for option in self.param.get_options(): self._input.append_text(option.get_name())
        self._input.set_active(self.param.get_option_keys().index(self.param.get_value()))
        self._input.connect('changed', self._apply_change)
        self.pack_start(self._input, False)
    def get_text(self): return self.param.get_option_keys()[self._input.get_active()]
    def set_tooltip_text(self, text): self._input.set_tooltip_text(text)

class EnumEntryParam(InputParam):
    """Provide an entry box and drop down menu for Raw Enum types."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = gtk.combo_box_entry_new_text()
        for option in self.param.get_options(): self._input.append_text(option.get_name())
        try: self._input.set_active(self.param.get_option_keys().index(self.param.get_value()))
        except:
            self._input.set_active(-1)
            self._input.get_child().set_text(self.param.get_value())
        self._input.connect('changed', self._apply_change)
        self._input.get_child().connect('changed', self._mark_changed)
        self._input.get_child().connect('focus-out-event', self._apply_change)
        self.pack_start(self._input, False)
    def get_text(self):
        if self._input.get_active() == -1: return self._input.get_child().get_text()
        return self.param.get_option_keys()[self._input.get_active()]
    def set_tooltip_text(self, text):
        if self._input.get_active() == -1: #custom entry
            self._input.get_child().set_tooltip_text(text)
        else: self._input.set_tooltip_text(text)
    def set_color(self, color):
        if self._input.get_active() == -1: #custom entry, use color
            self._input.get_child().modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))
            self._input.get_child().modify_text(gtk.STATE_NORMAL, Colors.PARAM_ENTRY_TEXT_COLOR)
        else: #from enum, make pale background
            self._input.get_child().modify_base(gtk.STATE_NORMAL, Colors.ENTRYENUM_CUSTOM_COLOR)
            self._input.get_child().modify_text(gtk.STATE_NORMAL, Colors.PARAM_ENTRY_TEXT_COLOR)


class FileParam(EntryParam):
    """Provide an entry box for filename and a button to browse for a file."""

    def __init__(self, *args, **kwargs):
        EntryParam.__init__(self, *args, **kwargs)
        input = gtk.Button('...')
        input.connect('clicked', self._handle_clicked)
        self.pack_start(input, False)

    def _handle_clicked(self, widget=None):
        """
        If the button was clicked, open a file dialog in open/save format.
        Replace the text in the entry with the new filename from the file dialog.
        """
        #get the paths
        file_path = self.param.is_valid() and self.param.get_evaluated() or ''
        (dirname, basename) = os.path.isfile(file_path) and os.path.split(file_path) or (file_path, '')
        if not os.path.exists(dirname): dirname = os.getcwd() #fix bad paths
        #build the dialog
        if self.param.get_type() == 'file_open':
            file_dialog = gtk.FileChooserDialog('Open a Data File...', None,
                gtk.FILE_CHOOSER_ACTION_OPEN, ('gtk-cancel',gtk.RESPONSE_CANCEL,'gtk-open',gtk.RESPONSE_OK))
        elif self.param.get_type() == 'file_save':
            file_dialog = gtk.FileChooserDialog('Save a Data File...', None,
                gtk.FILE_CHOOSER_ACTION_SAVE, ('gtk-cancel',gtk.RESPONSE_CANCEL, 'gtk-save',gtk.RESPONSE_OK))
            file_dialog.set_do_overwrite_confirmation(True)
            file_dialog.set_current_name(basename) #show the current filename
        file_dialog.set_current_folder(dirname) #current directory
        file_dialog.set_select_multiple(False)
        file_dialog.set_local_only(True)
        if gtk.RESPONSE_OK == file_dialog.run(): #run the dialog
            file_path = file_dialog.get_filename() #get the file path
            self._input.set_text(file_path)
            self._apply_change()
        file_dialog.destroy() #destroy the dialog


PARAM_MARKUP_TMPL="""\
#set $foreground = $param.is_valid() and 'black' or 'red'
<span foreground="$foreground" font_desc="$font"><b>$encode($param.get_name()): </b>$encode(repr($param))</span>"""

PARAM_LABEL_MARKUP_TMPL="""\
#set $foreground = $modified and 'blue' or $param.is_valid() and 'black' or 'red'
#set $underline = $has_cb and 'low' or 'none'
<span underline="$underline" foreground="$foreground" font_desc="Sans 9">$encode($param.get_name())</span>"""

TIP_MARKUP_TMPL="""\
########################################
#def truncate(string)
    #set $max_len = 100
    #set $string = str($string)
    #if len($string) > $max_len
$('%s...%s'%($string[:$max_len/2], $string[-$max_len/2:]))#slurp
    #else
$string#slurp
    #end if
#end def
########################################
Key: $param.get_key()
Type: $param.get_type()
#if $param.is_valid()
Value: $truncate($param.get_evaluated())
#elif len($param.get_error_messages()) == 1
Error: $(param.get_error_messages()[0])
#else
Error:
    #for $error_msg in $param.get_error_messages()
 * $error_msg
    #end for
#end if"""

class Param(Element):
    """The graphical parameter."""

    def __init__(self): Element.__init__(self)

    def get_input(self, *args, **kwargs):
        """
        Get the graphical gtk class to represent this parameter.
        An enum requires and combo parameter.
        A non-enum with options gets a combined entry/combo parameter.
        All others get a standard entry parameter.

        Returns:
            gtk input class
        """
        if self.get_type() in ('file_open', 'file_save'):
            return FileParam(self, *args, **kwargs)

        elif self.is_enum():
            return EnumParam(self, *args, **kwargs)

        elif self.get_options():
            return EnumEntryParam(self, *args, **kwargs)

        return EntryParam(self, *args, **kwargs)

    def get_markup(self):
        """
        Get the markup for this param.

        Returns:
            a pango markup string
        """
        return Utils.parse_template(PARAM_MARKUP_TMPL, param=self, font=PARAM_FONT)
