"""
Copyright 2007 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from os import path

from gi.repository import Gtk

from . import Constants, Utils, Dialogs


class FileDialogHelper(Gtk.FileChooserDialog, object):
    """
    A wrapper class for the gtk file chooser dialog.
    Implement a file chooser dialog with only necessary parameters.
    """
    title = ''
    action = Gtk.FileChooserAction.OPEN
    filter_label = ''
    filter_ext = ''

    def __init__(self, parent, current_file_path):
        """
        FileDialogHelper constructor.
        Create a save or open dialog with cancel and ok buttons.
        Use standard settings: no multiple selection, local files only, and the * filter.

        Args:
            action: Gtk.FileChooserAction.OPEN or Gtk.FileChooserAction.SAVE
            title: the title of the dialog (string)
        """
        ok_stock = {
            Gtk.FileChooserAction.OPEN: 'gtk-open',
            Gtk.FileChooserAction.SAVE: 'gtk-save'
        }[self.action]

        Gtk.FileChooserDialog.__init__(self, title=self.title, action=self.action,
                                       transient_for=parent)
        self.add_buttons('gtk-cancel', Gtk.ResponseType.CANCEL,
                         ok_stock, Gtk.ResponseType.OK)
        self.set_select_multiple(False)
        self.set_local_only(True)

        self.parent = parent
        self.current_file_path = current_file_path or path.join(
            Constants.DEFAULT_FILE_PATH, Constants.NEW_FLOGRAPH_TITLE + Constants.FILE_EXTENSION)

        self.set_current_folder(path.dirname(
            current_file_path))  # current directory
        self.setup_filters()

    def setup_filters(self, filters=None):
        set_default = True
        filters = filters or (
            [(self.filter_label, self.filter_ext)] if self.filter_label else [])
        filters.append(('All Files', ''))
        for label, ext in filters:
            if not label:
                continue
            f = Gtk.FileFilter()
            f.set_name(label)
            f.add_pattern('*' + ext)
            self.add_filter(f)
            if not set_default:
                self.set_filter(f)
                set_default = True

    def run(self):
        """Get the filename and destroy the dialog."""
        response = Gtk.FileChooserDialog.run(self)
        filename = self.get_filename() if response == Gtk.ResponseType.OK else None
        self.destroy()
        return filename


class SaveFileDialog(FileDialogHelper):
    """A dialog box to save or open flow graph files. This is a base class, do not use."""
    action = Gtk.FileChooserAction.SAVE

    def __init__(self, parent, current_file_path):
        super(SaveFileDialog, self).__init__(parent, current_file_path)
        self.set_current_name(path.splitext(path.basename(
            self.current_file_path))[0] + self.filter_ext)
        self.set_create_folders(True)
        self.set_do_overwrite_confirmation(True)


class OpenFileDialog(FileDialogHelper):
    """A dialog box to save or open flow graph files. This is a base class, do not use."""
    action = Gtk.FileChooserAction.OPEN

    def show_missing_message(self, filename):
        Dialogs.MessageDialogWrapper(
            self.parent,
            Gtk.MessageType.WARNING, Gtk.ButtonsType.CLOSE, 'Cannot Open!',
            'File <b>{filename}</b> Does not Exist!'.format(
                filename=Utils.encode(filename)),
        ).run_and_destroy()

    def get_filename(self):
        """
        Run the dialog and get the filename.
        If this is a save dialog and the file name is missing the extension, append the file extension.
        If the file name with the extension already exists, show a overwrite dialog.
        If this is an open dialog, return a list of filenames.

        Returns:
            the complete file path
        """
        filenames = Gtk.FileChooserDialog.get_filenames(self)
        for filename in filenames:
            if not path.exists(filename):
                self.show_missing_message(filename)
                return None  # rerun
        return filenames


class OpenFlowGraph(OpenFileDialog):
    title = 'Open a Flow Graph from a File...'
    filter_label = 'Flow Graph Files'
    filter_ext = Constants.FILE_EXTENSION

    def __init__(self, parent, current_file_path=''):
        super(OpenFlowGraph, self).__init__(parent, current_file_path)
        self.set_select_multiple(True)


class OpenQSS(OpenFileDialog):
    title = 'Open a QSS theme...'
    filter_label = 'QSS Themes'
    filter_ext = '.qss'


class SaveFlowGraph(SaveFileDialog):
    title = 'Save a Flow Graph to a File...'
    filter_label = 'Flow Graph Files'
    filter_ext = Constants.FILE_EXTENSION


class SaveConsole(SaveFileDialog):
    title = 'Save Console to a File...'
    filter_label = 'Test Files'
    filter_ext = '.txt'


class SaveScreenShot(SaveFileDialog):
    title = 'Save a Flow Graph Screen Shot...'
    filters = [('PDF Files', '.pdf'), ('PNG Files', '.png'),
               ('SVG Files', '.svg')]
    filter_ext = '.pdf'  # the default

    def __init__(self, parent, current_file_path=''):
        super(SaveScreenShot, self).__init__(parent, current_file_path)

        self.config = Gtk.Application.get_default().config

        self._button = button = Gtk.CheckButton(label='Background transparent')
        self._button.set_active(
            self.config.screen_shot_background_transparent())
        self.set_extra_widget(button)

    def setup_filters(self, filters=None):
        super(SaveScreenShot, self).setup_filters(self.filters)

    def show_missing_message(self, filename):
        Dialogs.MessageDialogWrapper(
            self.parent,
            Gtk.MessageType.ERROR, Gtk.ButtonsType.CLOSE, 'Can not Save!',
            'File Extension of <b>{filename}</b> not supported!'.format(
                filename=Utils.encode(filename)),
        ).run_and_destroy()

    def run(self):
        valid_exts = {ext for label, ext in self.filters}
        filename = None
        while True:
            response = Gtk.FileChooserDialog.run(self)
            if response != Gtk.ResponseType.OK:
                filename = None
                break

            filename = self.get_filename()
            if path.splitext(filename)[1] in valid_exts:
                break

            self.show_missing_message(filename)

        bg_transparent = self._button.get_active()
        self.config.screen_shot_background_transparent(bg_transparent)
        self.destroy()
        return filename, bg_transparent
