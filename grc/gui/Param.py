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

import os

import pygtk
pygtk.require('2.0')
import gtk

from . import Colors, Constants
from .Element import Element
from . import Utils

from ..core.Param import Param as _Param


class InputParam(gtk.HBox):
    """The base class for an input parameter inside the input parameters dialog."""
    expand = False

    def __init__(self, param, parent, changed_callback=None, editing_callback=None):
        gtk.HBox.__init__(self)
        self.param = param
        self._parent = parent
        self._changed_callback = changed_callback
        self._editing_callback = editing_callback
        self.label = gtk.Label() #no label, markup is added by set_markup
        self.label.set_size_request(Utils.scale_scalar(150), -1)
        self.pack_start(self.label, False)
        self.set_markup = lambda m: self.label.set_markup(m)
        self.tp = None
        self._have_pending_changes = False
        #connect events
        self.connect('show', self._update_gui)

    def set_color(self, color):
        pass

    def set_tooltip_text(self, text):
        pass

    def get_text(self):
        raise NotImplementedError()

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
            modified=self._have_pending_changes))
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
        self._have_pending_changes = True
        self._update_gui()
        if self._editing_callback:
            self._editing_callback(self, None)

    def _apply_change(self, *args):
        """
        Handle a gui change by setting the new param value,
        calling the callback (if applicable), and updating.
        """
        #set the new value
        self.param.set_value(self.get_text())
        #call the callback
        if self._changed_callback:
            self._changed_callback(self, None)
        else:
            self.param.validate()
        #gui update
        self._have_pending_changes = False
        self._update_gui()

    def _handle_key_press(self, widget, event):
        if event.keyval == gtk.keysyms.Return and event.state & gtk.gdk.CONTROL_MASK:
            self._apply_change(widget, event)
            return True
        return False

    def apply_pending_changes(self):
        if self._have_pending_changes:
            self._apply_change()


class EntryParam(InputParam):
    """Provide an entry box for strings and numbers."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = gtk.Entry()
        self._input.set_text(self.param.get_value())
        self._input.connect('changed', self._mark_changed)
        self._input.connect('focus-out-event', self._apply_change)
        self._input.connect('key-press-event', self._handle_key_press)
        self.pack_start(self._input, True)

    def get_text(self):
        return self._input.get_text()

    def set_color(self, color):
        need_status_color = self.label not in self.get_children()
        text_color = (
            None if not need_status_color else
            gtk.gdk.color_parse('blue') if self._have_pending_changes else
            gtk.gdk.color_parse('red') if not self.param.is_valid() else
            None)
        base_color = (
            Colors.BLOCK_DISABLED_COLOR
            if need_status_color and not self.param.get_parent().get_enabled()
            else gtk.gdk.color_parse(color)
        )
        self._input.modify_base(gtk.STATE_NORMAL, base_color)
        if text_color:
            self._input.modify_text(gtk.STATE_NORMAL, text_color)

    def set_tooltip_text(self, text):
        try:
            self._input.set_tooltip_text(text)
        except AttributeError:
            pass  # no tooltips for old GTK


class MultiLineEntryParam(InputParam):
    """Provide an multi-line box for strings."""
    expand = True

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._buffer = gtk.TextBuffer()
        self._buffer.set_text(self.param.get_value())
        self._buffer.connect('changed', self._mark_changed)

        self._view = gtk.TextView(self._buffer)
        self._view.connect('focus-out-event', self._apply_change)
        self._view.connect('key-press-event', self._handle_key_press)

        self._sw = gtk.ScrolledWindow()
        self._sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self._sw.add_with_viewport(self._view)

        self.pack_start(self._sw, True)

    def get_text(self):
        buf = self._buffer
        return buf.get_text(buf.get_start_iter(),
                            buf.get_end_iter()).strip()

    def set_color(self, color):
        self._view.modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))

    def set_tooltip_text(self, text):
        try:
            self._view.set_tooltip_text(text)
        except AttributeError:
            pass  # no tooltips for old GTK


# try:
#     import gtksourceview
#     lang_manager = gtksourceview.SourceLanguagesManager()
#     py_lang = lang_manager.get_language_from_mime_type('text/x-python')
#
#     class PythonEditorParam(InputParam):
#         expand = True
#
#         def __init__(self, *args, **kwargs):
#             InputParam.__init__(self, *args, **kwargs)
#
#             buf = self._buffer = gtksourceview.SourceBuffer()
#             buf.set_language(py_lang)
#             buf.set_highlight(True)
#             buf.set_text(self.param.get_value())
#             buf.connect('changed', self._mark_changed)
#
#             view = self._view = gtksourceview.SourceView(self._buffer)
#             view.connect('focus-out-event', self._apply_change)
#             view.connect('key-press-event', self._handle_key_press)
#             view.set_tabs_width(4)
#             view.set_insert_spaces_instead_of_tabs(True)
#             view.set_auto_indent(True)
#             view.set_border_width(2)
#
#             scroll = gtk.ScrolledWindow()
#             scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
#             scroll.add_with_viewport(view)
#             self.pack_start(scroll, True)
#
#         def get_text(self):
#             buf = self._buffer
#             return buf.get_text(buf.get_start_iter(),
#                                 buf.get_end_iter()).strip()
#
# except ImportError:
#     print "Package 'gtksourceview' not found. No Syntax highlighting."
#     PythonEditorParam = MultiLineEntryParam

class PythonEditorParam(InputParam):

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        button = self._button = gtk.Button('Open in Editor')
        button.connect('clicked', self.open_editor)
        self.pack_start(button, True)

    def open_editor(self, widget=None):
        flowgraph = self.param.get_parent().get_parent()
        flowgraph.install_external_editor(self.param, self._parent)

    def get_text(self):
        pass  # we never update the value from here

    def set_color(self, color):
        pass

    def _apply_change(self, *args):
        pass


class EnumParam(InputParam):
    """Provide an entry box for Enum types with a drop down menu."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = gtk.combo_box_new_text()
        for option in self.param.get_options(): self._input.append_text(option.get_name())
        self._input.set_active(self.param.get_option_keys().index(self.param.get_value()))
        self._input.connect('changed', self._editing_callback)
        self._input.connect('changed', self._apply_change)
        self.pack_start(self._input, False)

    def get_text(self):
        return self.param.get_option_keys()[self._input.get_active()]

    def set_tooltip_text(self, text):
        try:
            self._input.set_tooltip_text(text)
        except AttributeError:
            pass  # no tooltips for old GTK


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
        self._input.get_child().connect('key-press-event', self._handle_key_press)
        self.pack_start(self._input, False)

    def get_text(self):
        if self._input.get_active() == -1: return self._input.get_child().get_text()
        return self.param.get_option_keys()[self._input.get_active()]

    def set_tooltip_text(self, text):
        try:
            if self._input.get_active() == -1: #custom entry
                self._input.get_child().set_tooltip_text(text)
            else:
                self._input.set_tooltip_text(text)
        except AttributeError:
            pass  # no tooltips for old GTK

    def set_color(self, color):
        if self._input.get_active() == -1: #custom entry, use color
            self._input.get_child().modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))
        else: #from enum, make pale background
            self._input.get_child().modify_base(gtk.STATE_NORMAL, Colors.ENTRYENUM_CUSTOM_COLOR)


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
        # check for qss theme default directory
        if self.param.get_key() == 'qt_qss_theme':
            dirname = os.path.dirname(dirname)  # trim filename
            if not os.path.exists(dirname):
               platform = self.param.get_parent().get_parent().get_parent()
               dirname = os.path.join(platform.config.install_prefix,
                                      '/share/gnuradio/themes')
        if not os.path.exists(dirname):
            dirname = os.getcwd()  # fix bad paths

        #build the dialog
        if self.param.get_type() == 'file_open':
            file_dialog = gtk.FileChooserDialog('Open a Data File...', self._parent,
                gtk.FILE_CHOOSER_ACTION_OPEN, ('gtk-cancel',gtk.RESPONSE_CANCEL,'gtk-open',gtk.RESPONSE_OK))
        elif self.param.get_type() == 'file_save':
            file_dialog = gtk.FileChooserDialog('Save a Data File...', self._parent,
                gtk.FILE_CHOOSER_ACTION_SAVE, ('gtk-cancel',gtk.RESPONSE_CANCEL, 'gtk-save',gtk.RESPONSE_OK))
            file_dialog.set_do_overwrite_confirmation(True)
            file_dialog.set_current_name(basename) #show the current filename
        else:
            raise ValueError("Can't open file chooser dialog for type " + repr(self.param.get_type()))
        file_dialog.set_current_folder(dirname) #current directory
        file_dialog.set_select_multiple(False)
        file_dialog.set_local_only(True)
        if gtk.RESPONSE_OK == file_dialog.run(): #run the dialog
            file_path = file_dialog.get_filename() #get the file path
            self._input.set_text(file_path)
            self._editing_callback()
            self._apply_change()
        file_dialog.destroy() #destroy the dialog


PARAM_MARKUP_TMPL="""\
#set $foreground = $param.is_valid() and 'black' or 'red'
<span foreground="$foreground" font_desc="$font"><b>$encode($param.get_name()): </b>$encode(repr($param).replace('\\n',' '))</span>"""

PARAM_LABEL_MARKUP_TMPL="""\
#set $foreground = $modified and 'foreground="blue"' or not $param.is_valid() and 'foreground="red"' or ''
#set $underline = $has_cb and 'low' or 'none'
<span underline="$underline" $foreground font_desc="Sans 9">$encode($param.get_name())</span>"""

TIP_MARKUP_TMPL="""\
########################################
#def truncate(value)
    #set $max_len = 100
    #set $string = str(value)
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


class Param(Element, _Param):
    """The graphical parameter."""

    def __init__(self, **kwargs):
        Element.__init__(self)
        _Param.__init__(self, **kwargs)

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
            input_widget = FileParam(self, *args, **kwargs)

        elif self.is_enum():
            input_widget = EnumParam(self, *args, **kwargs)

        elif self.get_options():
            input_widget = EnumEntryParam(self, *args, **kwargs)

        elif self.get_type() == '_multiline':
            input_widget = MultiLineEntryParam(self, *args, **kwargs)

        elif self.get_type() == '_multiline_python_external':
            input_widget = PythonEditorParam(self, *args, **kwargs)

        else:
            input_widget = EntryParam(self, *args, **kwargs)

        return input_widget

    def get_markup(self):
        """
        Get the markup for this param.

        Returns:
            a pango markup string
        """
        return Utils.parse_template(PARAM_MARKUP_TMPL,
                                    param=self, font=Constants.PARAM_FONT)
