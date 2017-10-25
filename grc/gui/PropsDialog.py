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

import Actions
from Dialogs import SimpleTextDisplay
from Constants import MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT, FONT_SIZE
import Utils
import pango

TAB_LABEL_MARKUP_TMPL="""\
#set $foreground = not $valid and 'foreground="red"' or ''
<span $foreground>$encode($tab)</span>"""


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

    def __init__(self, block, parent):
        """
        Properties dialog constructor.

        Args:
            block: a block instance
        """
        self._hash = 0

        gtk.Dialog.__init__(
            self,
            title='Properties: %s' % block.get_name(),
            parent=parent,
            buttons=(gtk.STOCK_OK, gtk.RESPONSE_ACCEPT,
                     gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT,
                     gtk.STOCK_APPLY, gtk.RESPONSE_APPLY)
        )
        self.set_response_sensitive(gtk.RESPONSE_APPLY, False)
        self.set_size_request(*Utils.scale(
            (MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT)
        ))
        self._block = block
        self._parent = parent

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
        self._docs_text_display = doc_view = SimpleTextDisplay()
        doc_view.get_buffer().create_tag('b', weight=pango.WEIGHT_BOLD)
        self._docs_box = gtk.ScrolledWindow()
        self._docs_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self._docs_box.add_with_viewport(self._docs_text_display)
        notebook.append_page(self._docs_box, gtk.Label("Documentation"))

        # Generated code for the block
        if Actions.TOGGLE_SHOW_CODE_PREVIEW_TAB.get_active():
            self._code_text_display = code_view = SimpleTextDisplay()
            code_view.set_wrap_mode(gtk.WRAP_NONE)
            code_view.get_buffer().create_tag('b', weight=pango.WEIGHT_BOLD)
            code_view.modify_font(pango.FontDescription(
                'monospace %d' % FONT_SIZE))
            code_box = gtk.ScrolledWindow()
            code_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
            code_box.add_with_viewport(self._code_text_display)
            notebook.append_page(code_box, gtk.Label("Generated Code"))
        else:
            self._code_text_display = None

        # Error Messages for the block
        self._error_messages_text_display = SimpleTextDisplay()
        self._error_box = gtk.ScrolledWindow()
        self._error_box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self._error_box.add_with_viewport(self._error_messages_text_display)
        vpaned.pack2(self._error_box)
        vpaned.set_position(int(0.65 * MIN_DIALOG_HEIGHT))

        # Connect events
        self.connect('key-press-event', self._handle_key_press)
        self.connect('show', self.update_gui)
        self.connect('response', self._handle_response)
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
        # create a tuple of things from each param that affects the params box
        self._hash = hash(tuple([(
            hash(param), param.get_name(), param.get_type(),
            param.get_hide() == 'all',
        ) for param in self._block.get_params()]))
        return self._hash != old_hash

    def _handle_changed(self, *args):
        """
        A change occurred within a param:
        Rewrite/validate the block and update the gui.
        """
        # update for the block
        self._block.rewrite()
        self._block.validate()
        self.update_gui()

    def _activate_apply(self, *args):
        self.set_response_sensitive(gtk.RESPONSE_APPLY, True)

    def update_gui(self, widget=None, force=False):
        """
        Repopulate the parameters boxes (if changed).
        Update all the input parameters.
        Update the error messages box.
        Hide the box if there are no errors.
        Update the documentation block.
        Hide the box if there are no docs.
        """
        # update the params box
        if force or self._params_changed():
            # hide params box before changing
            for tab, label, vbox in self._params_boxes:
                vbox.hide_all()
                # empty the params box
                for child in vbox.get_children():
                    vbox.remove(child)
                    child.destroy()
                # repopulate the params box
                box_all_valid = True
                for param in filter(lambda p: p.get_tab_label() == tab, self._block.get_params()):
                    if param.get_hide() == 'all':
                        continue
                    box_all_valid = box_all_valid and param.is_valid()
                    input_widget = param.get_input(self, self._handle_changed, self._activate_apply)
                    vbox.pack_start(input_widget, input_widget.expand)
                label.set_markup(Utils.parse_template(TAB_LABEL_MARKUP_TMPL, valid=box_all_valid, tab=tab))
                # show params box with new params
                vbox.show_all()
        # update the errors box
        if self._block.is_valid():
            self._error_box.hide()
        else:
            self._error_box.show()
        messages = '\n\n'.join(self._block.get_error_messages())
        self._error_messages_text_display.set_text(messages)
        # update the docs box
        self._update_docs_page()
        # update the generated code
        self._update_generated_code_page()

    def _update_docs_page(self):
        """Show documentation from XML and try to display best matching docstring"""
        buffer = self._docs_text_display.get_buffer()
        buffer.delete(buffer.get_start_iter(), buffer.get_end_iter())
        pos = buffer.get_end_iter()

        docstrings = self._block.get_doc()
        if not docstrings:
            return

        # show documentation string from block xml
        from_xml = docstrings.pop('', '')
        for line in from_xml.splitlines():
            if line.lstrip() == line and line.endswith(':'):
                buffer.insert_with_tags_by_name(pos, line + '\n', 'b')
            else:
                buffer.insert(pos, line + '\n')
        if from_xml:
            buffer.insert(pos, '\n')

        # if given the current parameters an exact match can be made
        block_constructor = self._block.get_make().rsplit('.', 2)[-1]
        block_class = block_constructor.partition('(')[0].strip()
        if block_class in docstrings:
            docstrings = {block_class: docstrings[block_class]}

        # show docstring(s) extracted from python sources
        for cls_name, docstring in docstrings.iteritems():
            buffer.insert_with_tags_by_name(pos, cls_name + '\n', 'b')
            buffer.insert(pos, docstring + '\n\n')
        pos.backward_chars(2)
        buffer.delete(pos, buffer.get_end_iter())

    def _update_generated_code_page(self):
        if not self._code_text_display:
            return  # user disabled code preview

        buffer = self._code_text_display.get_buffer()
        block = self._block
        key = block.get_key()

        if key == 'epy_block':
            src = block.get_param('_source_code').get_value()
        elif key == 'epy_module':
            src = block.get_param('source_code').get_value()
        else:
            src = ''

        def insert(header, text):
            if not text:
                return
            buffer.insert_with_tags_by_name(buffer.get_end_iter(), header, 'b')
            buffer.insert(buffer.get_end_iter(), text)

        buffer.delete(buffer.get_start_iter(), buffer.get_end_iter())
        insert('# Imports\n', '\n'.join(block.get_imports()))
        if key.startswith('variable'):
            insert('\n\n# Variables\n', block.get_var_make())
        insert('\n\n# Blocks\n', block.get_make())
        if src:
            insert('\n\n# External Code ({0}.py)\n'.format(block.get_id()), src)

    def _handle_key_press(self, widget, event):
        """
        Handle key presses from the keyboard.
        Call the ok response when enter is pressed.

        Returns:
            false to forward the keypress
        """
        if (event.keyval == gtk.keysyms.Return and
            event.state & gtk.gdk.CONTROL_MASK == 0 and
            not isinstance(widget.get_focus(), gtk.TextView)
        ):
            self.response(gtk.RESPONSE_ACCEPT)
            return True  # handled here
        return False  # forward the keypress

    def _handle_response(self, widget, response):
        if response in (gtk.RESPONSE_APPLY, gtk.RESPONSE_ACCEPT):
            for tab, label, vbox in self._params_boxes:
                for child in vbox.get_children():
                    child.apply_pending_changes()
            self.set_response_sensitive(gtk.RESPONSE_APPLY, False)
            return True
        return False


