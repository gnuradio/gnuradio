"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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

import pygtk
pygtk.require('2.0')
import gtk
import Utils
import Actions


class SimpleTextDisplay(gtk.TextView):
    """A non editable gtk text view."""

    def __init__(self, text=''):
        """
        TextDisplay constructor.

        Args:
            text: the text to display (string)
        """
        text_buffer = gtk.TextBuffer()
        text_buffer.set_text(text)
        self.set_text = text_buffer.set_text
        gtk.TextView.__init__(self, text_buffer)
        self.set_editable(False)
        self.set_cursor_visible(False)
        self.set_wrap_mode(gtk.WRAP_WORD_CHAR)


class TextDisplay(SimpleTextDisplay):

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
        # make backspaces work
        line = self._consume_backspaces(line)
        # add the remaining text to buffer
        self.get_buffer().insert(self.get_buffer().get_end_iter(), line)
        # Automatically scroll on insert
        self.scroll_to_end()

    def _consume_backspaces(self, line):
        """removes text from the buffer if line starts with \b*"""
        if not line: return
        # for each \b delete one char from the buffer
        back_count = 0
        start_iter = self.get_buffer().get_end_iter()
        while line[back_count] == '\b':
            # stop at the beginning of a line
            if not start_iter.starts_line(): start_iter.backward_char()
            back_count += 1
        # remove chars
        self.get_buffer().delete(start_iter, self.get_buffer().get_end_iter())
        # return remaining text
        return line[back_count:]

    def scroll_to_end(self):
        if self.scroll_lock:
            buffer = self.get_buffer()
            buffer.move_mark(buffer.get_insert(), buffer.get_end_iter())
            self.scroll_to_mark(buffer.get_insert(), 0.0)

    def clear(self):
        buffer = self.get_buffer()
        buffer.delete(buffer.get_start_iter(), buffer.get_end_iter())

    def save(self, file_path):
        report_file = open(file_path, 'w')
        buffer = self.get_buffer()
        report_file.write(buffer.get_text(buffer.get_start_iter(), buffer.get_end_iter(), True))
        report_file.close()

    # Callback functions to handle the scrolling lock and clear context menus options
    # Action functions are set by the ActionHandler's init function
    def clear_cb(self, menu_item, web_view):
        Actions.CLEAR_REPORTS()

    def scroll_back_cb(self, menu_item, web_view):
        Actions.TOGGLE_SCROLL_LOCK()

    def save_cb(self, menu_item, web_view):
        Actions.SAVE_REPORTS()

    def populate_popup(self, view, menu):
        """Create a popup menu for the scroll lock and clear functions"""
        menu.append(gtk.SeparatorMenuItem())

        lock = gtk.CheckMenuItem("Scroll Lock")
        menu.append(lock)
        lock.set_active(self.scroll_lock)
        lock.connect('activate', self.scroll_back_cb, view)

        save = gtk.ImageMenuItem(gtk.STOCK_SAVE)
        menu.append(save)
        save.connect('activate', self.save_cb, view)

        clear = gtk.ImageMenuItem(gtk.STOCK_CLEAR)
        menu.append(clear)
        clear.connect('activate', self.clear_cb, view)
        menu.show_all()
        return False


def MessageDialogHelper(type, buttons, title=None, markup=None, default_response=None, extra_buttons=None):
    """
    Create a modal message dialog and run it.

    Args:
        type: the type of message: gtk.MESSAGE_INFO, gtk.MESSAGE_WARNING, gtk.MESSAGE_QUESTION or gtk.MESSAGE_ERROR
        buttons: the predefined set of buttons to use:
        gtk.BUTTONS_NONE, gtk.BUTTONS_OK, gtk.BUTTONS_CLOSE, gtk.BUTTONS_CANCEL, gtk.BUTTONS_YES_NO, gtk.BUTTONS_OK_CANCEL

    Args:
        title: the title of the window (string)
        markup: the message text with pango markup
        default_response: if set, determines which button is highlighted by default
        extra_buttons: a tuple containing pairs of values; each value is the button's text and the button's return value

    Returns:
        the gtk response from run()
    """
    message_dialog = gtk.MessageDialog(None, gtk.DIALOG_MODAL, type, buttons)
    if title: message_dialog.set_title(title)
    if markup: message_dialog.set_markup(markup)
    if extra_buttons: message_dialog.add_buttons(*extra_buttons)
    if default_response: message_dialog.set_default_response(default_response)
    response = message_dialog.run()
    message_dialog.destroy()
    return response


ERRORS_MARKUP_TMPL="""\
#for $i, $err_msg in enumerate($errors)
<b>Error $i:</b>
$encode($err_msg.replace('\t', '  '))

#end for"""
def ErrorsDialog(flowgraph): MessageDialogHelper(
    type=gtk.MESSAGE_ERROR,
    buttons=gtk.BUTTONS_CLOSE,
    title='Flow Graph Errors',
    markup=Utils.parse_template(ERRORS_MARKUP_TMPL, errors=flowgraph.get_error_messages()),
)

class AboutDialog(gtk.AboutDialog):
    """A cute little about dialog."""

    def __init__(self, platform):
        """AboutDialog constructor."""
        gtk.AboutDialog.__init__(self)
        self.set_name(platform.get_name())
        self.set_version(platform.get_version())
        self.set_license(platform.get_license())
        self.set_copyright(platform.get_license().splitlines()[0])
        self.set_website(platform.get_website())
        self.run()
        self.destroy()

def HelpDialog(): MessageDialogHelper(
    type=gtk.MESSAGE_INFO,
    buttons=gtk.BUTTONS_CLOSE,
    title='Help',
    markup="""\
<b>Usage Tips</b>

<u>Add block</u>: drag and drop or double click a block in the block selection window.
<u>Rotate block</u>: Select a block, press left/right on the keyboard.
<u>Change type</u>: Select a block, press up/down on the keyboard.
<u>Edit parameters</u>: double click on a block in the flow graph.
<u>Make connection</u>: click on the source port of one block, then click on the sink port of another block.
<u>Remove connection</u>: select the connection and press delete, or drag the connection.

* See the menu for other keyboard shortcuts.""")

COLORS_DIALOG_MARKUP_TMPL = """\
<b>Color Mapping</b>

#if $colors
    #set $max_len = max([len(color[0]) for color in $colors]) + 10
    #for $title, $color_spec in $colors
<span background="$color_spec"><tt>$($encode($title).center($max_len))</tt></span>
    #end for
#end if
"""

def TypesDialog(platform): MessageDialogHelper(
    type=gtk.MESSAGE_INFO,
    buttons=gtk.BUTTONS_CLOSE,
    title='Types',
    markup=Utils.parse_template(COLORS_DIALOG_MARKUP_TMPL, colors=platform.get_colors()))
