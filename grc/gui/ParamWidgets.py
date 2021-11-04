# Copyright 2007-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import os
import configparser
import subprocess

from gi.repository import Gtk, Gdk

from . import Constants
from . import Utils
from .canvas.colors import LIGHT_THEME_STYLES, DARK_THEME_STYLES


def have_dark_theme():
    """
    Returns true if the currently selected theme is a dark one.
    """
    def is_dark_theme(theme_name):
        """
        Check if a theme is dark based on its name.
        """
        return theme_name and (theme_name in Constants.GTK_DARK_THEMES or "dark" in theme_name.lower())
    # GoGoGo
    config = configparser.ConfigParser()
    config.read(os.path.expanduser(Constants.GTK_SETTINGS_INI_PATH))
    prefer_dark = config.get(
        'Settings', Constants.GTK_INI_PREFER_DARK_KEY, fallback=None)
    theme_name = config.get(
        'Settings', Constants.GTK_INI_THEME_NAME_KEY, fallback=None)
    if prefer_dark in ('1', 'yes', 'true', 'on') or is_dark_theme(theme_name):
        return True
    try:
        theme = subprocess.check_output(
            ["gsettings", "get", "org.gnome.desktop.interface", "gtk-theme"],
            stderr=subprocess.DEVNULL
        ).decode('utf-8').strip().replace("'", "")
    except:
        return False
    return is_dark_theme(theme)


def add_style_provider():
    """
    Load GTK styles
    """
    style_provider = Gtk.CssProvider()
    dark_theme = have_dark_theme()
    style_provider.load_from_data(
        DARK_THEME_STYLES if dark_theme else LIGHT_THEME_STYLES)
    Gtk.StyleContext.add_provider_for_screen(
        Gdk.Screen.get_default(),
        style_provider,
        Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
    )


add_style_provider()


class InputParam(Gtk.HBox):
    """The base class for an input parameter inside the input parameters dialog."""
    expand = False

    def __init__(self, param, changed_callback=None, editing_callback=None, transient_for=None):
        Gtk.HBox.__init__(self)

        self.param = param
        self._changed_callback = changed_callback
        self._editing_callback = editing_callback
        self._transient_for = transient_for

        self.label = Gtk.Label()
        self.label.set_size_request(Utils.scale_scalar(150), -1)
        self.label.show()
        self.pack_start(self.label, False, False, 0)

        self.tp = None
        self._have_pending_changes = False

        self.connect('show', self._update_gui)

    def set_color(self, css_name):
        pass

    def set_tooltip_text(self, text):
        pass

    def get_text(self):
        raise NotImplementedError()

    def _update_gui(self, *args):
        """
        Set the markup, color, tooltip, show/hide.
        """
        self.label.set_markup(
            self.param.format_label_markup(self._have_pending_changes))
        self.set_color('dtype_' + self.param.dtype)

        self.set_tooltip_text(self.param.format_tooltip_text())

        if self.param.hide == 'all':
            self.hide()
        else:
            self.show_all()

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
        # set the new value
        self.param.set_value(self.get_text())
        # call the callback
        if self._changed_callback:
            self._changed_callback(self, None)
        else:
            self.param.validate()
        # gui update
        self._have_pending_changes = False
        self._update_gui()

    def _handle_key_press(self, widget, event):
        if event.keyval == Gdk.KEY_Return and event.get_state() & Gdk.ModifierType.CONTROL_MASK:
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
        self._input = Gtk.Entry()
        self._input.set_text(self.param.get_value())
        self._input.connect('changed', self._mark_changed)
        self._input.connect('focus-out-event', self._apply_change)
        self._input.connect('key-press-event', self._handle_key_press)
        self.pack_start(self._input, True, True, 0)

    def get_text(self):
        return self._input.get_text()

    def set_color(self, css_name):
        self._input.set_name(css_name)

    def set_tooltip_text(self, text):
        self._input.set_tooltip_text(text)


class MultiLineEntryParam(InputParam):
    """Provide an multi-line box for strings."""
    expand = True

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._buffer = Gtk.TextBuffer()
        self._buffer.set_text(self.param.get_value())
        self._buffer.connect('changed', self._mark_changed)

        self._view = Gtk.TextView()
        self._view.set_buffer(self._buffer)
        self._view.connect('focus-out-event', self._apply_change)
        self._view.connect('key-press-event', self._handle_key_press)

        self._sw = Gtk.ScrolledWindow()
        self._sw.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        self._sw.set_shadow_type(type=Gtk.ShadowType.IN)
        self._sw.add(self._view)

        self.pack_start(self._sw, True, True, True)

    def get_text(self):
        buf = self._buffer
        text = buf.get_text(buf.get_start_iter(), buf.get_end_iter(),
                            include_hidden_chars=False)
        return text.strip()

    def set_color(self, css_name):
        self._view.set_name(css_name)

    def set_tooltip_text(self, text):
        self._view.set_tooltip_text(text)


class PythonEditorParam(InputParam):

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        button = self._button = Gtk.Button(label='Open in Editor')
        button.connect('clicked', self.open_editor)
        self.pack_start(button, True, True, True)

    def open_editor(self, widget=None):
        self.param.parent_flowgraph.install_external_editor(
            self.param, parent=self._transient_for)

    def get_text(self):
        pass  # we never update the value from here

    def _apply_change(self, *args):
        pass


class EnumParam(InputParam):
    """Provide an entry box for Enum types with a drop down menu."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = Gtk.ComboBoxText()
        for option_name in self.param.options.values():
            self._input.append_text(option_name)

        self.param_values = list(self.param.options)
        self._input.set_active(self.param_values.index(self.param.get_value()))
        self._input.connect('changed', self._editing_callback)
        self._input.connect('changed', self._apply_change)
        self.pack_start(self._input, False, False, 0)

    def get_text(self):
        return self.param_values[self._input.get_active()]

    def set_tooltip_text(self, text):
        self._input.set_tooltip_text(text)


class EnumEntryParam(InputParam):
    """Provide an entry box and drop down menu for Raw Enum types."""

    def __init__(self, *args, **kwargs):
        InputParam.__init__(self, *args, **kwargs)
        self._input = Gtk.ComboBoxText.new_with_entry()
        for option_name in self.param.options.values():
            self._input.append_text(option_name)

        self.param_values = list(self.param.options)
        value = self.param.get_value()
        try:
            self._input.set_active(self.param_values.index(value))
        except ValueError:
            self._input.set_active(-1)
            self._input.get_child().set_text(value)

        self._input.connect('changed', self._apply_change)
        self._input.get_child().connect('changed', self._mark_changed)
        self._input.get_child().connect('focus-out-event', self._apply_change)
        self._input.get_child().connect('key-press-event', self._handle_key_press)
        self.pack_start(self._input, False, False, 0)

    @property
    def has_custom_value(self):
        return self._input.get_active() == -1

    def get_text(self):
        if self.has_custom_value:
            return self._input.get_child().get_text()
        else:
            return self.param_values[self._input.get_active()]

    def set_tooltip_text(self, text):
        if self.has_custom_value:  # custom entry
            self._input.get_child().set_tooltip_text(text)
        else:
            self._input.set_tooltip_text(text)

    def set_color(self, css_name):
        self._input.get_child().set_name(
            css_name if not self.has_custom_value else 'enum_custom'
        )


class FileParam(EntryParam):
    """Provide an entry box for filename and a button to browse for a file."""

    def __init__(self, *args, **kwargs):
        EntryParam.__init__(self, *args, **kwargs)
        self._open_button = Gtk.Button(label='...')
        self._open_button.connect('clicked', self._handle_clicked)
        self.pack_start(self._open_button, False, False, 0)

    def _handle_clicked(self, widget=None):
        """
        If the button was clicked, open a file dialog in open/save format.
        Replace the text in the entry with the new filename from the file dialog.
        """
        # get the paths
        file_path = self.param.is_valid() and self.param.get_evaluated() or ''
        (dirname, basename) = os.path.isfile(
            file_path) and os.path.split(file_path) or (file_path, '')
        # check for qss theme default directory
        if self.param.key == 'qt_qss_theme':
            dirname = os.path.dirname(dirname)  # trim filename
            if not os.path.exists(dirname):
                config = self.param.parent_platform.config
                dirname = os.path.join(
                    config.install_prefix, '/share/gnuradio/themes')
        if not os.path.exists(dirname):
            dirname = os.getcwd()  # fix bad paths

        # build the dialog
        if self.param.dtype == 'file_open':
            file_dialog = Gtk.FileChooserDialog(
                title='Open a Data File...', action=Gtk.FileChooserAction.OPEN,
                transient_for=self._transient_for,
            )
            file_dialog.add_buttons(
                'gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-open', Gtk.ResponseType.OK)
        elif self.param.dtype == 'file_save':
            file_dialog = Gtk.FileChooserDialog(
                title='Save a Data File...', action=Gtk.FileChooserAction.SAVE,
                transient_for=self._transient_for,
            )
            file_dialog.add_buttons(
                'gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-save', Gtk.ResponseType.OK)
            file_dialog.set_do_overwrite_confirmation(True)
            file_dialog.set_current_name(basename)  # show the current filename
        else:
            raise ValueError(
                "Can't open file chooser dialog for type " + repr(self.param.dtype))
        file_dialog.set_current_folder(dirname)  # current directory
        file_dialog.set_select_multiple(False)
        file_dialog.set_local_only(True)
        if Gtk.ResponseType.OK == file_dialog.run():  # run the dialog
            file_path = file_dialog.get_filename()  # get the file path
            self._input.set_text(file_path)
            self._editing_callback()
            self._apply_change()
        file_dialog.destroy()  # destroy the dialog


class DirectoryParam(FileParam):
    """Provide an entry box for a directory and a button to browse for it."""

    def _handle_clicked(self, widget=None):
        """
        Open the directory selector, when the button is clicked.
        On success, update the entry.
        """
        dirname = self.param.get_evaluated() if self.param.is_valid() else ''

        # Check if directory exists, if not fall back to workdir
        if not os.path.isdir(dirname):
            dirname = os.getcwd()

        if self.param.dtype == "dir_select":  # Setup directory selection dialog, and fail for unexpected dtype
            dir_dialog = Gtk.FileChooserDialog(
                title='Select a Directory...', action=Gtk.FileChooserAction.SELECT_FOLDER,
                transient_for=self._transient_for
            )
        else:
            raise ValueError(
                "Can't open directory chooser dialog for type " + repr(self.param.dtype))

        # Set dialog properties
        dir_dialog.add_buttons(
            'gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-open', Gtk.ResponseType.OK)
        dir_dialog.set_current_folder(dirname)
        dir_dialog.set_local_only(True)
        dir_dialog.set_select_multiple(False)

        # Show dialog and update parameter on success
        if Gtk.ResponseType.OK == dir_dialog.run():
            path = dir_dialog.get_filename()
            self._input.set_text(path)
            self._editing_callback()
            self._apply_change()

        # Cleanup dialog
        dir_dialog.destroy()
