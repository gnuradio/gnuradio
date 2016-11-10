"""
Copyright 2015 Free Software Foundation, Inc.
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

from operator import attrgetter

import pygtk
pygtk.require('2.0')
import gtk
import gobject

from . import Actions
from . import Preferences
from .Constants import DEFAULT_BLOCKS_WINDOW_WIDTH

BLOCK_INDEX = 0
ID_INDEX = 1


class VariableEditorContextMenu(gtk.Menu):
    """ A simple context menu for our variable editor """
    def __init__(self, var_edit):
        gtk.Menu.__init__(self)

        self.imports = gtk.MenuItem("Add _Import")
        self.imports.connect('activate', var_edit.handle_action, var_edit.ADD_IMPORT)
        self.add(self.imports)

        self.variables = gtk.MenuItem("Add _Variable")
        self.variables.connect('activate', var_edit.handle_action, var_edit.ADD_VARIABLE)
        self.add(self.variables)
        self.add(gtk.SeparatorMenuItem())

        self.enable = gtk.MenuItem("_Enable")
        self.enable.connect('activate', var_edit.handle_action, var_edit.ENABLE_BLOCK)
        self.disable = gtk.MenuItem("_Disable")
        self.disable.connect('activate', var_edit.handle_action, var_edit.DISABLE_BLOCK)
        self.add(self.enable)
        self.add(self.disable)
        self.add(gtk.SeparatorMenuItem())

        self.delete = gtk.MenuItem("_Delete")
        self.delete.connect('activate', var_edit.handle_action, var_edit.DELETE_BLOCK)
        self.add(self.delete)
        self.add(gtk.SeparatorMenuItem())

        self.properties = gtk.MenuItem("_Properties...")
        self.properties.connect('activate', var_edit.handle_action, var_edit.OPEN_PROPERTIES)
        self.add(self.properties)
        self.show_all()

    def update_sensitive(self, selected, enabled=False):
        self.delete.set_sensitive(selected)
        self.properties.set_sensitive(selected)
        self.enable.set_sensitive(selected and not enabled)
        self.disable.set_sensitive(selected and enabled)


class VariableEditor(gtk.VBox):

    # Actions that are handled by the editor
    ADD_IMPORT = 0
    ADD_VARIABLE = 1
    OPEN_PROPERTIES = 2
    DELETE_BLOCK = 3
    DELETE_CONFIRM = 4
    ENABLE_BLOCK = 5
    DISABLE_BLOCK = 6

    def __init__(self, platform, get_flow_graph):
        gtk.VBox.__init__(self)
        self.platform = platform
        self.get_flow_graph = get_flow_graph
        self._block = None
        self._mouse_button_pressed = False

        # Only use the model to store the block reference and name.
        # Generate everything else dynamically
        self.treestore = gtk.TreeStore(gobject.TYPE_PYOBJECT,  # Block reference
                                       gobject.TYPE_STRING)    # Category and block name
        self.treeview = gtk.TreeView(self.treestore)
        self.treeview.set_enable_search(False)
        self.treeview.set_search_column(-1)
        #self.treeview.set_enable_search(True)
        #self.treeview.set_search_column(ID_INDEX)
        self.treeview.get_selection().set_mode('single')
        self.treeview.set_headers_visible(True)
        self.treeview.connect('button-press-event', self._handle_mouse_button_press)
        self.treeview.connect('button-release-event', self._handle_mouse_button_release)
        self.treeview.connect('motion-notify-event', self._handle_motion_notify)
        self.treeview.connect('key-press-event', self._handle_key_button_press)

        # Block Name or Category
        self.id_cell = gtk.CellRendererText()
        self.id_cell.connect('edited', self._handle_name_edited_cb)
        id_column = gtk.TreeViewColumn("Id", self.id_cell, text=ID_INDEX)
        id_column.set_name("id")
        id_column.set_resizable(True)
        id_column.set_max_width(300)
        id_column.set_min_width(80)
        id_column.set_fixed_width(100)
        id_column.set_sizing(gtk.TREE_VIEW_COLUMN_FIXED)
        id_column.set_cell_data_func(self.id_cell, self.set_properties)
        self.id_column = id_column
        self.treeview.append_column(id_column)
        self.treestore.set_sort_column_id(ID_INDEX, gtk.SORT_ASCENDING)
        # For forcing resize
        self._col_width = 0

        # Block Value
        self.value_cell = gtk.CellRendererText()
        self.value_cell.connect('edited', self._handle_value_edited_cb)
        value_column = gtk.TreeViewColumn("Value", self.value_cell)
        value_column.set_name("value")
        value_column.set_resizable(False)
        value_column.set_expand(True)
        value_column.set_min_width(100)
        value_column.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
        value_column.set_cell_data_func(self.value_cell, self.set_value)
        self.value_column = value_column
        self.treeview.append_column(value_column)

        # Block Actions (Add, Remove)
        self.action_cell = gtk.CellRendererPixbuf()
        value_column.pack_start(self.action_cell, False)
        value_column.set_cell_data_func(self.action_cell, self.set_icon)

        # Make the scrolled window to hold the tree view
        scrolled_window = gtk.ScrolledWindow()
        scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scrolled_window.add_with_viewport(self.treeview)
        scrolled_window.set_size_request(DEFAULT_BLOCKS_WINDOW_WIDTH, -1)
        self.pack_start(scrolled_window)

        # Context menus
        self._context_menu = VariableEditorContextMenu(self)
        self._confirm_delete = Preferences.variable_editor_confirm_delete()

    # Sets cell contents
    def set_icon(self, col, cell, model, iter):
        block = model.get_value(iter, BLOCK_INDEX)
        if block:
            pb = self.treeview.render_icon(gtk.STOCK_CLOSE, gtk.ICON_SIZE_MENU, None)
        else:
            pb = self.treeview.render_icon(gtk.STOCK_ADD, gtk.ICON_SIZE_MENU, None)
        cell.set_property('pixbuf', pb)

    def set_value(self, col, cell, model, iter):
        sp = cell.set_property
        block = model.get_value(iter, BLOCK_INDEX)

        # Set the default properties for this column first.
        # Some set in set_properties() may be overridden (editable for advanced variable blocks)
        self.set_properties(col, cell, model, iter)

        # Set defaults
        value = None
        self.set_tooltip_text(None)

        # Block specific values
        if block:
            if block.get_key() == 'import':
                value = block.get_param('import').get_value()
            elif block.get_key() != "variable":
                value = "<Open Properties>"
                sp('editable', False)
                sp('foreground', '#0D47A1')
            else:
                value = block.get_param('value').get_value()

            # Check if there are errors in the blocks.
            # Show the block error as a tooltip
            error_message = block.get_error_messages()
            if len(error_message) > 0:
                # Set the error message to the last error in the list.
                # This should be the first message generated
                self.set_tooltip_text(error_message[-1])
            else:
                # Evaluate and show the value (if it is a variable)
                if block.get_key() == "variable":
                    evaluated = str(block.get_param('value').evaluate())
                    self.set_tooltip_text(evaluated)
        # Always set the text value.
        sp('text', value)

    def set_properties(self, col, cell, model, iter):
        sp = cell.set_property
        block = model.get_value(iter, BLOCK_INDEX)
        # Set defaults
        sp('sensitive', True)
        sp('editable', False)
        sp('foreground', None)

        # Block specific changes
        if block:
            if not block.get_enabled():
                # Disabled block. But, this should still be editable
                sp('editable', True)
                sp('foreground', 'gray')
            else:
                sp('editable', True)
            if block.get_error_messages():
                sp('foreground', 'red')

    def update_gui(self):
        if not self.get_flow_graph():
            return
        self._update_blocks()
        self._rebuild()
        self.treeview.expand_all()

    def _update_blocks(self):
        self._imports = filter(attrgetter('is_import'),
                               self.get_flow_graph().blocks)
        self._variables = filter(attrgetter('is_variable'),
                                 self.get_flow_graph().blocks)

    def _rebuild(self, *args):
        self.treestore.clear()
        imports = self.treestore.append(None, [None, 'Imports'])
        variables = self.treestore.append(None, [None, 'Variables'])
        for block in self._imports:
            self.treestore.append(imports, [block, block.get_param('id').get_value()])
        for block in sorted(self._variables, key=lambda v: v.get_id()):
            self.treestore.append(variables, [block, block.get_param('id').get_value()])

    def _handle_name_edited_cb(self, cell, path, new_text):
        block = self.treestore[path][BLOCK_INDEX]
        block.get_param('id').set_value(new_text)
        Actions.VARIABLE_EDITOR_UPDATE()

    def _handle_value_edited_cb(self, cell, path, new_text):
        block = self.treestore[path][BLOCK_INDEX]
        if block.is_import:
            block.get_param('import').set_value(new_text)
        else:
            block.get_param('value').set_value(new_text)
        Actions.VARIABLE_EDITOR_UPDATE()

    def handle_action(self, item, key, event=None):
        """
        Single handler for the different actions that can be triggered by the context menu,
        key presses or mouse clicks. Also triggers an update of the flow graph and editor.
        """
        if key == self.ADD_IMPORT:
            self.get_flow_graph().add_new_block('import')
        elif key == self.ADD_VARIABLE:
            self.get_flow_graph().add_new_block('variable')
        elif key == self.OPEN_PROPERTIES:
            Actions.BLOCK_PARAM_MODIFY(self._block)
        elif key == self.DELETE_BLOCK:
            self.get_flow_graph().remove_element(self._block)
        elif key == self.DELETE_CONFIRM:
            if self._confirm_delete:
                # Create a context menu to confirm the delete operation
                confirmation_menu = gtk.Menu()
                block_id = self._block.get_param('id').get_value().replace("_", "__")
                confirm = gtk.MenuItem("Delete {0}".format(block_id))
                confirm.connect('activate', self.handle_action, self.DELETE_BLOCK)
                confirmation_menu.add(confirm)
                confirmation_menu.show_all()
                confirmation_menu.popup(None, None, None, event.button, event.time)
            else:
                self.handle_action(None, self.DELETE_BLOCK, None)
        elif key == self.ENABLE_BLOCK:
            self._block.set_enabled(True)
        elif key == self.DISABLE_BLOCK:
            self._block.set_enabled(False)
        Actions.VARIABLE_EDITOR_UPDATE()

    def _handle_mouse_button_press(self, widget, event):
        """
        Handles mouse button for several different events:
        - Double Click to open properties for advanced blocks
        - Click to add/remove blocks
        """
        # Save the column width to see if it changes on button_release
        self._mouse_button_pressed = True
        self._col_width = self.id_column.get_width()

        path = widget.get_path_at_pos(int(event.x), int(event.y))
        if path:
            # If there is a valid path, then get the row, column and block selected.
            row = self.treestore[path[0]]
            col = path[1]
            self._block = row[BLOCK_INDEX]

            if event.button == 1 and col.get_name() == "value":
                # Make sure this has a block (not the import/variable rows)
                if self._block and event.type == gtk.gdk._2BUTTON_PRESS:
                    # Open the advanced dialog if it is a gui variable
                    if self._block.get_key() not in ("variable", "import"):
                        self.handle_action(None, self.OPEN_PROPERTIES, event=event)
                        return True
                if event.type == gtk.gdk.BUTTON_PRESS:
                    # User is adding/removing blocks
                    # Make sure this is the action cell (Add/Remove Icons)
                    if path[2] > col.cell_get_position(self.action_cell)[0]:
                        if row[1] == "Imports":
                            # Add a new import block.
                            self.handle_action(None, self.ADD_IMPORT, event=event)
                        elif row[1] == "Variables":
                            # Add a new variable block
                            self.handle_action(None, self.ADD_VARIABLE, event=event)
                        else:
                            self.handle_action(None, self.DELETE_CONFIRM, event=event)
                        return True
            elif event.button == 3 and event.type == gtk.gdk.BUTTON_PRESS:
                if self._block:
                    self._context_menu.update_sensitive(True, enabled=self._block.get_enabled())
                else:
                    self._context_menu.update_sensitive(False)
                self._context_menu.popup(None, None, None, event.button, event.time)

            # Null handler. Stops the treeview from handling double click events.
            if event.type == gtk.gdk._2BUTTON_PRESS:
                return True
        return False

    def _handle_mouse_button_release(self, widget, event):
        self._mouse_button_pressed = False
        return False

    def _handle_motion_notify(self, widget, event):
        # Check to see if the column size has changed
        if self._mouse_button_pressed and self.id_column.get_width() != self._col_width:
            self.value_column.queue_resize()
        return False

    def _handle_key_button_press(self, widget, event):
        model, path = self.treeview.get_selection().get_selected_rows()
        if path and self._block:
            if self._block.get_enabled() and event.string == "d":
                self.handle_action(None, self.DISABLE_BLOCK, None)
                return True
            elif not self._block.get_enabled() and event.string == "e":
                self.handle_action(None, self.ENABLE_BLOCK, None)
                return True
        return False
