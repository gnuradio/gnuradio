# Copyright 2008, 2009, 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import sys
import textwrap
from distutils.spawn import find_executable

from gi.repository import Gtk, GLib, Gdk, Pango

from . import Utils, Actions, Constants
from ..core import Messages

from enum import IntEnum
import ast

class SimpleTextDisplay(Gtk.TextView):
    """
    A non user-editable gtk text view.
    """

    def __init__(self, text=''):
        """
        TextDisplay constructor.

        Args:
            text: the text to display (string)
        """
        Gtk.TextView.__init__(self)
        self.set_text = self.get_buffer().set_text
        self.set_text(text)
        self.set_editable(False)
        self.set_cursor_visible(False)
        self.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)


class TextDisplay(SimpleTextDisplay):
    """
    A non user-editable scrollable text view with popup menu.
    """

    def __init__(self, text=''):
        """
        TextDisplay constructor.

        Args:
            text: the text to display (string)
        """
        SimpleTextDisplay.__init__(self, text)
        self.scroll_lock = True
        self.connect("populate-popup", self.populate_popup)

    def insert(self, line):
        """
        Append text after handling backspaces and auto-scroll.

        Args:
            line: the text to append (string)
        """
        line = self._consume_backspaces(line)
        self.get_buffer().insert(self.get_buffer().get_end_iter(), line)
        self.scroll_to_end()

    def _consume_backspaces(self, line):
        """
        Removes text from the buffer if line starts with '\b'

        Args:
            line: a string which may contain backspaces

        Returns:
            The string that remains from 'line' with leading '\b's removed.
        """
        if not line:
            return

        # for each \b delete one char from the buffer
        back_count = 0
        start_iter = self.get_buffer().get_end_iter()
        while len(line) > back_count and line[back_count] == '\b':
            # stop at the beginning of a line
            if not start_iter.starts_line():
                start_iter.backward_char()
            back_count += 1
        # remove chars from buffer
        self.get_buffer().delete(start_iter, self.get_buffer().get_end_iter())
        return line[back_count:]

    def scroll_to_end(self):
        """ Update view's scroll position. """
        if self.scroll_lock:
            buf = self.get_buffer()
            mark = buf.get_insert()
            buf.move_mark(mark, buf.get_end_iter())
            self.scroll_mark_onscreen(mark)

    def clear(self):
        """ Clear all text from buffer. """
        buf = self.get_buffer()
        buf.delete(buf.get_start_iter(), buf.get_end_iter())

    def save(self, file_path):
        """
        Save context of buffer to the given file.

        Args:
            file_path: location to save buffer contents
        """
        with open(file_path, 'w') as logfile:
            buf = self.get_buffer()
            logfile.write(buf.get_text(buf.get_start_iter(),
                                       buf.get_end_iter(), True))

    # Action functions are set by the Application's init function
    def clear_cb(self, menu_item, web_view):
        """ Callback function to clear the text buffer """
        Actions.CLEAR_CONSOLE()

    def scroll_back_cb(self, menu_item, web_view):
        """ Callback function to toggle scroll lock """
        Actions.TOGGLE_SCROLL_LOCK()

    def save_cb(self, menu_item, web_view):
        """ Callback function to save the buffer """
        Actions.SAVE_CONSOLE()

    def populate_popup(self, view, menu):
        """Create a popup menu for the scroll lock and clear functions"""
        menu.append(Gtk.SeparatorMenuItem())

        lock = Gtk.CheckMenuItem(label="Scroll Lock")
        menu.append(lock)
        lock.set_active(self.scroll_lock)
        lock.connect('activate', self.scroll_back_cb, view)

        save = Gtk.ImageMenuItem(label="Save Console")
        menu.append(save)
        save.connect('activate', self.save_cb, view)

        clear = Gtk.ImageMenuItem(label="Clear Console")
        menu.append(clear)
        clear.connect('activate', self.clear_cb, view)
        menu.show_all()
        return False


class MessageDialogWrapper(Gtk.MessageDialog):
    """ Run a message dialog. """

    def __init__(self, parent, message_type, buttons, title=None, markup=None,
                 default_response=None, extra_buttons=None):
        """
        Create a modal message dialog.

        Args:
            message_type: the type of message may be one of:
                            Gtk.MessageType.INFO
                            Gtk.MessageType.WARNING
                            Gtk.MessageType.QUESTION or Gtk.MessageType.ERROR
            buttons: the predefined set of buttons to use:
                            Gtk.ButtonsType.NONE
                            Gtk.ButtonsType.OK
                            Gtk.ButtonsType.CLOSE
                            Gtk.ButtonsType.CANCEL
                            Gtk.ButtonsType.YES_NO
                            Gtk.ButtonsType.OK_CANCEL
            title: the title of the window (string)
            markup: the message text with pango markup
            default_response: if set, determines which button is highlighted by default
            extra_buttons: a tuple containing pairs of values:
                            each value is the button's text and the button's return value

        """
        Gtk.MessageDialog.__init__(
            self, transient_for=parent, modal=True, destroy_with_parent=True,
            message_type=message_type, buttons=buttons
        )
        if title:
            self.set_title(title)
        if markup:
            self.set_markup(markup)
        if extra_buttons:
            self.add_buttons(*extra_buttons)
        if default_response:
            self.set_default_response(default_response)

    def run_and_destroy(self):
        response = self.run()
        self.hide()
        return response


class ErrorsDialog(Gtk.Dialog):
    """ Display flowgraph errors. """

    def __init__(self, parent, flowgraph):
        """Create a listview of errors"""
        Gtk.Dialog.__init__(
            self,
            title='Errors and Warnings',
            transient_for=parent,
            modal=True,
            destroy_with_parent=True,
        )
        self.clipboard = Gtk.Clipboard.get(Gdk.SELECTION_CLIPBOARD)
        self.add_buttons(Gtk.STOCK_OK, Gtk.ResponseType.ACCEPT)
        self.set_size_request(750, Constants.MIN_DIALOG_HEIGHT)
        self.set_border_width(10)

        self.store = Gtk.ListStore(str, str, str)
        self.update(flowgraph)

        self.treeview = Gtk.TreeView(model=self.store)
        self.treeview.connect("button_press_event", self.mouse_click)
        for i, column_title in enumerate(["Block", "Aspect", "Message"]):
            renderer = Gtk.CellRendererText()
            column = Gtk.TreeViewColumn(column_title, renderer, text=i)
            column.set_sort_column_id(i)  # liststore id matches treeview id
            column.set_resizable(True)
            self.treeview.append_column(column)

        self.scrollable = Gtk.ScrolledWindow()
        self.scrollable.set_vexpand(True)
        self.scrollable.add(self.treeview)

        self.vbox.pack_start(self.scrollable, True, True, 0)
        self.show_all()

    def update(self, flowgraph):
        self.store.clear()
        for element, message in flowgraph.iter_error_messages():
            if element.is_block:
                src, aspect = element.name, ''
            elif element.is_connection:
                src = element.source_block.name
                aspect = "Connection to '{}'".format(element.sink_block.name)
            elif element.is_port:
                src = element.parent_block.name
                aspect = "{} '{}'".format('Sink' if element.is_sink else 'Source', element.name)
            elif element.is_param:
                src = element.parent_block.name
                aspect = "Param '{}'".format(element.name)
            else:
                src = aspect = ''
            self.store.append([src, aspect, message])

    def run_and_destroy(self):
        response = self.run()
        self.hide()
        return response

    def mouse_click(self, _, event):
        """ Handle mouse click, so user can copy the error message """
        if event.button == 3:
            path_info = self.treeview.get_path_at_pos(event.x, event.y)
            if path_info is not None:
                path, col, _, _ = path_info
                self.treeview.grab_focus()
                self.treeview.set_cursor(path, col, 0)

            selection = self.treeview.get_selection()
            (model, iterator) = selection.get_selected()
            self.clipboard.set_text(model[iterator][2], -1)
            print(model[iterator][2])


def show_about(parent, config):
    ad = Gtk.AboutDialog(transient_for=parent)
    ad.set_program_name(config.name)
    ad.set_name('')
    ad.set_license(config.license)

    py_version = sys.version.split()[0]
    ad.set_version("{} (Python {})".format(config.version, py_version))

    try:
        ad.set_logo(Gtk.IconTheme().load_icon('gnuradio-grc', 64, 0))
    except GLib.Error:
        Messages.send("Failed to set window logo\n")

    #ad.set_comments("")
    ad.set_copyright(config.license.splitlines()[0])
    ad.set_website(config.website)

    ad.connect("response", lambda action, param: action.hide())
    ad.show()


def show_help(parent):
    """ Display basic usage tips. """
    markup = textwrap.dedent("""\
        <b>Usage Tips</b>
        \n\
        <u>Add block</u>: drag and drop or double click a block in the block
       selection window.
        <u>Rotate block</u>: Select a block, press left/right on the keyboard.
        <u>Change type</u>: Select a block, press up/down on the keyboard.
        <u>Edit parameters</u>: double click on a block in the flow graph.
        <u>Make connection</u>: click on the source port of one block, then
       click on the sink port of another block.
        <u>Remove connection</u>: select the connection and press delete, or
       drag the connection.
        \n\
        *Press Ctrl+K or see menu for Keyboard - Shortcuts
        \
    """)
    markup = markup.replace("Ctrl", Utils.get_modifier_key())

    MessageDialogWrapper(
        parent, Gtk.MessageType.INFO, Gtk.ButtonsType.CLOSE, title='Help', markup=markup
    ).run_and_destroy()

def show_keyboard_shortcuts(parent):
    """ Display keyboard shortcut-keys. """
    markup = textwrap.dedent("""\
    <b>Keyboard Shortcuts</b>
    \n\
    <u>Ctrl+N</u>: Create a new flowgraph.
    <u>Ctrl+O</u>: Open an existing flowgraph.
    <u>Ctrl+S</u>: Save the current flowgraph or save as for new.
    <u>Ctrl+W</u>: Close the current flowgraph.
    <u>Ctrl+Z</u>: Undo a change to the flowgraph.
    <u>Ctrl+Y</u>: Redo a change to the flowgraph.
    <u>Ctrl+A</u>: Selects all blocks and connections.
    <u>Ctrl+P</u>: Screen Capture of the Flowgraph.
    <u>Ctrl+Shift+P</u>: Save the console output to file.
    <u>Ctrl+L</u>: Clear the console.
    <u>Ctrl+E</u>: Show variable editor.
    <u>Ctrl+F</u>: Search for a block by name.
    <u>Ctrl+Q</u>: Quit.
    <u>F1</u>    : Help menu.
    <u>F5</u>    : Generate the Flowgraph.
    <u>F6</u>    : Execute the Flowgraph.
    <u>F7</u>    : Kill the Flowgraph.
    <u>Ctrl+Shift+S</u>: Save as the current flowgraph.
    <u>Ctrl+Shift+D</u>: Create a duplicate of current flow graph.

    <u>Ctrl+X/C/V</u>: Edit-cut/copy/paste.
    <u>Ctrl+D/B/R</u>: Toggle visibility of disabled blocks or
            connections/block tree widget/console.
    <u>Shift+T/M/B/L/C/R</u>: Vertical Align Top/Middle/Bottom and
            Horizontal Align Left/Center/Right respectively of the
            selected block.
    \
    """)
    markup = markup.replace("Ctrl", Utils.get_modifier_key())

    MessageDialogWrapper(
        parent, Gtk.MessageType.INFO, Gtk.ButtonsType.CLOSE, title='Keyboard - Shortcuts', markup=markup
    ).run_and_destroy()


def show_get_involved(parent):
    """Get Involved Instructions"""
    markup = textwrap.dedent("""\
    <b>Welcome to GNU Radio Community!</b>
    \n\
    For more details on contributing to GNU Radio and getting engaged with our great community visit <a href="https://wiki.gnuradio.org/index.php/HowToGetInvolved">here</a>.
    \n\
    You can also join our <a href="https://chat.gnuradio.org/">Matrix chat server</a>, IRC Channel (#gnuradio) or contact through our <a href="https://lists.gnu.org/mailman/listinfo/discuss-gnuradio">mailing list (discuss-gnuradio)</a>.
    \
    """)

    MessageDialogWrapper(
        parent, Gtk.MessageType.QUESTION, Gtk.ButtonsType.CLOSE, title='Get - Involved', markup=markup
    ).run_and_destroy()


def show_types(parent):
    """ Display information about standard data types. """
    colors = [(name, color) for name, key, sizeof, color in Constants.CORE_TYPES]
    max_len = 10 + max(len(name) for name, code in colors)

    message = '\n'.join(
        '<span background="{color}"><tt>{name}</tt></span>'
        ''.format(color=color, name=Utils.encode(name).center(max_len))
        for name, color in colors
    )

    MessageDialogWrapper(
        parent, Gtk.MessageType.INFO, Gtk.ButtonsType.CLOSE, title='Types - Color Mapping', markup=message
    ).run_and_destroy()


def show_missing_xterm(parent, xterm):
    markup = textwrap.dedent("""\
        The xterm executable {0!r} is missing.
        You can change this setting in your gnuradio.conf, in section [grc], 'xterm_executable'.
        \n\
        (This message is shown only once)\
    """).format(xterm)

    MessageDialogWrapper(
        parent, message_type=Gtk.MessageType.WARNING, buttons=Gtk.ButtonsType.OK,
        title='Warning: missing xterm executable', markup=markup
    ).run_and_destroy()


def choose_editor(parent, config):
    """
    Give the option to either choose an editor or use the default.
    """
    if config.editor and find_executable(config.editor):
        return config.editor

    buttons = (
        'Choose Editor', Gtk.ResponseType.YES,
        'Use Default', Gtk.ResponseType.NO,
        Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL
    )
    response = MessageDialogWrapper(
        parent, message_type=Gtk.MessageType.QUESTION, buttons=Gtk.ButtonsType.NONE,
        title='Choose Editor', markup='Would you like to choose the editor to use?',
        default_response=Gtk.ResponseType.YES, extra_buttons=buttons
    ).run_and_destroy()

    # Handle the initial default/choose/cancel response
    # User wants to choose the editor to use
    editor = ''
    if response == Gtk.ResponseType.YES:
        file_dialog = Gtk.FileChooserDialog(
            'Select an Editor...', None,
            Gtk.FileChooserAction.OPEN,
            ('gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-open', Gtk.ResponseType.OK),
            transient_for=parent
        )
        file_dialog.set_select_multiple(False)
        file_dialog.set_local_only(True)
        file_dialog.set_current_folder('/usr/bin')
        try:
            if file_dialog.run() == Gtk.ResponseType.OK:
                editor = file_dialog.get_filename()
        finally:
            file_dialog.hide()

    # Go with the default editor
    elif response == Gtk.ResponseType.NO:
        try:
            process = None
            if sys.platform.startswith('linux'):
                process = find_executable('xdg-open')
            elif sys.platform.startswith('darwin'):
                process = find_executable('open')
            if process is None:
                raise ValueError("Can't find default editor executable")
            # Save
            editor = config.editor = process
        except Exception:
            Messages.send('>>> Unable to load the default editor. Please choose an editor.\n')

    if editor == '':
        Messages.send('>>> No editor selected.\n')
    return editor

class TrustAnswer(IntEnum):
    DO_NOT_TRUST = 1
    TRUST_ONCE = 2
    TRUST_FOREVER = 3

def trust_prompt(parent, config, flowgraph, message):
    """
    Get user response from a trust prompt dialog.
    Return false if user trusts the flow graph, else return true.
    """
    response = TrustPrompt(parent, flowgraph, message).run_and_destroy()

    if response == TrustAnswer.TRUST_FOREVER:
        files = config.get_trusted_files()
        files.append(flowgraph.grc_file_path)
        config.set_trusted_files(files)

    return not response in (TrustAnswer.TRUST_ONCE, TrustAnswer.TRUST_FOREVER)

class TrustPrompt(Gtk.Dialog):
    """ Prompt the user to trust the flow graph (once or forever)."""

    def __init__(self, parent, flowgraph, message):
        """
        TrustPrompt constructor.
        Args:
            flowgraph: the flowgraph which should be trusted
            message: the message text displayed by the prompt
        """
        Gtk.Dialog.__init__(
            self,
            title='Do you trust this flowgraph?',
            transient_for=parent,
            modal=True,
            destroy_with_parent=True,
        )

        buttons = (
            'Trust once', TrustAnswer.TRUST_ONCE,
            'Trust forever', TrustAnswer.TRUST_FOREVER,
            Gtk.STOCK_CANCEL, TrustAnswer.DO_NOT_TRUST
        )

        self.flowgraph = flowgraph

        self.add_buttons(*buttons)
        self.dialog_height = 300
        self.set_size_request(Constants.MIN_DIALOG_WIDTH, self.dialog_height)
        self.set_border_width(10)
        self.counter = 0

        # show first so Gtk sets dialog width and wraps text accordingly
        self.show_all()

        message_label = Gtk.Label(message)
        message_label.set_line_wrap(True)

        href = 'https://wiki.gnuradio.org/index.php/GRC:_View-Only_Mode'
        wiki_link = f'<a href="{href}">More information</a>'
        wiki_link_label = Gtk.Label(use_markup=True)
        wiki_link_label.set_markup(wiki_link)

        self.store = Gtk.ListStore(str, str, str)
        self.treeview = Gtk.TreeView(model=self.store)
        for i, column_title in enumerate(["Expression", "Origin", "Name"]):
            renderer = Gtk.CellRendererText()
            renderer.set_property("ellipsize", Pango.EllipsizeMode.END)
            column = Gtk.TreeViewColumn(column_title, renderer, text=i)
            column.set_sort_column_id(i)  # liststore id matches treeview id

            if i == 0:
                column.set_expand(True)
            if i > 0:
                column.set_fixed_width(100)

            column.set_resizable(True)
            self.treeview.append_column(column)

        self.scrollable = Gtk.ScrolledWindow()
        self.scrollable.set_vexpand(True)
        self.scrollable.add(self.treeview)

        self.counter_label = Gtk.Label()
        self.counter_label.set_xalign(0.0)

        self.overview_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        self.overview_box.pack_start(self.scrollable, True, True, 0)
        self.overview_box.pack_start(self.counter_label, False, True, 10)

        self.expander = Gtk.Expander(label="Expression Overview")
        self.expander.add(self.overview_box)
        self.expander.connect('activate', self.show_overview)

        self.vbox.pack_start(message_label, False, True, 10)
        self.vbox.pack_start(wiki_link_label, False, True, 10)
        self.vbox.pack_start(self.expander, False, True, 10)

        self.show_all()

    def is_literal(self, value):
        try:
            ast.literal_eval(value)
            return True
        except Exception:
            return False

    def show_overview(self, widget):
        """ Fill the prompt's expression overview with values fetched from the flowgraph """
        self.set_size_request(Constants.MIN_DIALOG_WIDTH, 500)
        self.store.clear()
        self.counter = 0

        params = []
        imports = []
        blocks = []

        for block in self.flowgraph.blocks:
            imps = block.templates.render('imports')
            if imps:
                imports += [(imps, block.name)]

            if block._eval_cache:
                for expr in block._eval_cache.keys():
                    if expr and not self.is_literal(expr):
                        blocks += [(expr, block.name)]

            for _, param in block.params.items():
                value = param.to_code()
                if value and not self.is_literal(value):
                    # surround non-code with quotes
                    try:
                        ast.parse(value)
                    except SyntaxError:
                        value = repr(value)

                    name = block.name + "." + param.name
                    params += [(value, name)]

        for value, name in params:
            self.store.append([value, "FLOW GRAPH", name])
        for value, name in imports:
            self.store.append([value, "BLOCK DEFINITION", name])
        for value, name in blocks:
            self.store.append([value, "BLOCK DEFINITION", name])

        self.counter = len(params) + len(imports) + len(blocks)
        self.counter_label.set_text(f"Number of expressions: {self.counter}")

    def run_and_destroy(self):
        response = self.run()
        self.hide()
        return response

class TrustManagerAnswer(IntEnum):
    ADD = 1
    ADD_DIR = 2
    REMOVE = 3
    CANCEL = 4

class TrustManagerDialog(Gtk.Dialog):
    """ Add or remove trusted flowgraphs or directories """

    def __init__(self, parent, config, platform):
        """
        Create a modal trust manage dialog.

        Args:
            config: the GRC config object
        """
        self.config = config
        self.platform = platform

        Gtk.Dialog.__init__(
            self,
            title='Manage trusted flowgraphs',
            transient_for=parent,
            modal=True,
            destroy_with_parent=True,
        )

        buttons = (
            'Add flow graph', TrustManagerAnswer.ADD,
            'Add directory', TrustManagerAnswer.ADD_DIR,
            'Remove', TrustManagerAnswer.REMOVE,
            Gtk.STOCK_CANCEL, TrustManagerAnswer.CANCEL
        )
        self.add_buttons(*buttons)
        self.connect('response', self._handle_response)
        self.set_size_request(Constants.MIN_DIALOG_WIDTH, Constants.MIN_DIALOG_HEIGHT)
        self.set_border_width(10)

        self.store = Gtk.ListStore(str)
        self.treeview = Gtk.TreeView(model=self.store)

        for file in config.get_trusted_files():
            self.store.append([file])

        for i, column_title in enumerate(['Path']):
            renderer = Gtk.CellRendererText()
            renderer.set_property("ellipsize", Pango.EllipsizeMode.END)
            column = Gtk.TreeViewColumn(column_title, renderer, text=i)
            self.treeview.append_column(column)

        self.scrollable = Gtk.ScrolledWindow()
        self.scrollable.set_vexpand(True)
        self.scrollable.add(self.treeview)

        self.vbox.pack_start(self.scrollable, True, True, 0)
        self.show_all()

    def _handle_response(self, widget, response):
        files = self.config.get_trusted_files()

        if response == TrustManagerAnswer.ADD:
            file_dialog = Gtk.FileChooserDialog(
                'Select grc-files to trust', None,
                Gtk.FileChooserAction.OPEN,
                ('gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-open', Gtk.ResponseType.OK),
                transient_for=self
            )
            file_dialog.set_select_multiple(True)
            file_dialog.set_local_only(True)
            grc_filter = Gtk.FileFilter()
            grc_filter.add_pattern("*.grc")
            file_dialog.add_filter(grc_filter)

            try:
                if file_dialog.run() == Gtk.ResponseType.OK:
                    files_to_add = file_dialog.get_filenames()
            finally:
                file_dialog.hide()

            files += files_to_add

        elif response == TrustManagerAnswer.ADD_DIR:
            file_dialog = Gtk.FileChooserDialog(
                'Select a directory to trust', None,
                Gtk.FileChooserAction.SELECT_FOLDER,
                ('gtk-cancel', Gtk.ResponseType.CANCEL, 'gtk-open', Gtk.ResponseType.OK),
                transient_for=self
            )
            file_dialog.set_local_only(True)

            try:
                if file_dialog.run() == Gtk.ResponseType.OK:
                    files_to_add = file_dialog.get_filenames()
            finally:
                file_dialog.hide()

            files += files_to_add

        elif response == TrustManagerAnswer.REMOVE:
            tm, ti = self.treeview.get_selection().get_selected()
            path = tm.get_value(ti, 0)

            try:
                files.remove(path)
                tm.remove(ti)
                self.platform.trusted_flowgraphs.discard(path)
            except ValueError:
                pass
        else:
            return False

        self.store.clear()

        files = list(set(files))
        for file in files:
            self.treeview.get_model().append([file])
            self.platform.trusted_flowgraphs.add(file)

        self.config.set_trusted_files(files)
        self.config.save()

        return True

    def run_and_destroy(self):
        response = self.run()

        while response in (TrustManagerAnswer.ADD, TrustManagerAnswer.ADD_DIR, TrustManagerAnswer.REMOVE):
            response = self.run()

        self.hide()
        return response
