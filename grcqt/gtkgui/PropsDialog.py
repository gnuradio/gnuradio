"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
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

from Dialogs import TextDisplay
from Constants import MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT
import Utils

TAB_LABEL_MARKUP_TMPL="""\
#set $foreground = $valid and 'black' or 'red'
<span foreground="$foreground">$encode($tab)</span>"""


def get_title_label(title):
    """
    Get a title label for the params window.
    The title will be bold, underlined, and left justified.

    Args:
        title: the text of the title

    Returns:
        a gtk object
    """
    label = gtk.Label()
    label.set_markup('\n<b><span underline="low">%s</span>:</b>\n'%title)
    hbox = gtk.HBox()
    hbox.pack_start(label, False, False, padding=11)
    return hbox


class PropsDialog(gtk.Dialog):
    """
    A dialog to set block parameters, view errors, and view documentation.
    """

    def __init__(self, block):
        """
        Properties dialog constructor.

        Args:
            block: a block instance
        """
        self._hash = 0

        gtk.Dialog.__init__(
            self,
            title='Properties: %s' % block.get_name(),
            buttons=(gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT, gtk.STOCK_OK, gtk.RESPONSE_ACCEPT),
        )
        self._block = block
        self.set_size_request(MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT)

        vpaned = gtk.VPaned()
        self.vbox.pack_start(vpaned)

        # Notebook to hold param boxes
        notebook = gtk.Notebook()
        notebook.set_show_border(False)
        notebook.set_scrollable(True)  # scroll arrows for page tabs
        notebook.set_tab_pos(gtk.POS_TOP)
        vpaned.pack1(notebook, True)

        # Params boxes for block parameters
        self._params_boxes = list()
        for tab in block.get_param_tab_labels():
            label = gtk.Label()
            vbox = gtk.VBox()
            scroll_box = gtk.ScrolledWindow()
            scroll_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
            scroll_box.add_with_viewport(vbox)
            notebook.append_page(scroll_box, label)
            self._params_boxes.append((tab, label, vbox))

        # Docs for the block
        self._docs_text_display = TextDisplay()
        self._docs_box = gtk.ScrolledWindow()
        self._docs_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self._docs_box.add_with_viewport(self._docs_text_display)
        notebook.append_page(self._docs_box, gtk.Label("Documentation"))

        # Error Messages for the block
        self._error_messages_text_display = TextDisplay()
        self._error_box = gtk.ScrolledWindow()
        self._error_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self._error_box.add_with_viewport(self._error_messages_text_display)
        vpaned.pack2(self._error_box)
        vpaned.set_position(int(0.65 * MIN_DIALOG_HEIGHT))

        # Connect events
        self.connect('key-press-event', self._handle_key_press)
        self.connect('show', self._update_gui)
        self.show_all()  # show all (performs initial gui update)

    def _params_changed(self):
        """
        Have the params in this dialog changed?
        Ex: Added, removed, type change, hide change...
        To the props dialog, the hide setting of 'none' and 'part' are identical.
        Therefore, the props dialog only cares if the hide setting is/not 'all'.
        Make a hash that uniquely represents the params' state.

        Returns:
            true if changed
        """
        old_hash = self._hash
        #create a tuple of things from each param that affects the params box
        self._hash = hash(tuple([(
            hash(param), param.get_type(), param.get_hide() == 'all',
        ) for param in self._block.get_params()]))
        return self._hash != old_hash

    def _handle_changed(self, *args):
        """
        A change occurred within a param:
        Rewrite/validate the block and update the gui.
        """
        #update for the block
        self._block.rewrite()
        self._block.validate()
        self._update_gui()

    def _update_gui(self, *args):
        """
        Repopulate the parameters boxes (if changed).
        Update all the input parameters.
        Update the error messages box.
        Hide the box if there are no errors.
        Update the documentation block.
        Hide the box if there are no docs.
        """
        #update the params box
        if self._params_changed():
            #hide params box before changing
            for tab, label, vbox in self._params_boxes:
                vbox.hide_all()
                # empty the params box
                vbox.forall(lambda c: vbox.remove(c) or c.destroy())
                # repopulate the params box
                box_all_valid = True
                for param in filter(lambda p: p.get_tab_label() == tab, self._block.get_params()):
                    if param.get_hide() == 'all':
                        continue
                    box_all_valid = box_all_valid and param.is_valid()
                    vbox.pack_start(param.get_input(self._handle_changed), False)
                label.set_markup(Utils.parse_template(TAB_LABEL_MARKUP_TMPL, valid=box_all_valid, tab=tab))
                #show params box with new params
                vbox.show_all()
        #update the errors box
        if self._block.is_valid():
            self._error_box.hide()
        else:
            self._error_box.show()
        messages = '\n\n'.join(self._block.get_error_messages())
        self._error_messages_text_display.set_text(messages)
        #update the docs box
        self._docs_text_display.set_text(self._block.get_doc())

    def _handle_key_press(self, widget, event):
        """
        Handle key presses from the keyboard.
        Call the ok response when enter is pressed.

        Returns:
            false to forward the keypress
        """
        if event.keyval == gtk.keysyms.Return:
            self.response(gtk.RESPONSE_ACCEPT)
            return True  # handled here
        return False  # forward the keypress

    def run(self):
        """
        Run the dialog and get its response.

        Returns:
            true if the response was accept
        """
        response = gtk.Dialog.run(self)
        self.destroy()
        return response == gtk.RESPONSE_ACCEPT
