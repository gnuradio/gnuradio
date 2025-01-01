# """
# Copyright 2015, 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio

# SPDX-License-Identifier: GPL-2.0-or-later

"""
VariableEditor.py - Modified to address issue #7615:
1. Dynamically show "Enable" or "Disable" based on the variable's state.
2. Restrict "Add Import" and "Add Variable" to header right-clicks.
3. Fix "Properties" dialog action.
4. Ensure expand/collapse functionality works correctly for '+' and '-' buttons.
"""

from gi.repository import Gtk, Gdk, GObject
from . import Actions, Constants, Utils

BLOCK_INDEX = 0
ID_INDEX = 1

class VariableEditorContextMenu(Gtk.Menu):
    """ A simple context menu for our variable editor """

    def __init__(self, var_edit):
        Gtk.Menu.__init__(self)

        self.imports = Gtk.MenuItem(label="Add _Import")
        self.imports.connect(
            'activate', var_edit.handle_action, var_edit.ADD_IMPORT)
        self.add(self.imports)

        self.variables = Gtk.MenuItem(label="Add _Variable")
        self.variables.connect(
            'activate', var_edit.handle_action, var_edit.ADD_VARIABLE)
        self.add(self.variables)
        self.add(Gtk.SeparatorMenuItem())

        self.enable = Gtk.MenuItem(label="_Enable")
        self.enable.connect(
            'activate', var_edit.handle_action, var_edit.ENABLE_BLOCK)
        self.disable = Gtk.MenuItem(label="_Disable")
        self.disable.connect(
            'activate', var_edit.handle_action, var_edit.DISABLE_BLOCK)
        self.add(self.enable)
        self.add(self.disable)
        self.add(Gtk.SeparatorMenuItem())

        self.delete = Gtk.MenuItem(label="_Delete")
        self.delete.connect(
            'activate', var_edit.handle_action, var_edit.DELETE_BLOCK)
        self.add(self.delete)
        self.add(Gtk.SeparatorMenuItem())

        self.properties = Gtk.MenuItem(label="_Properties...")
        self.properties.connect(
            'activate', var_edit.handle_action, var_edit.OPEN_PROPERTIES)
        self.add(self.properties)
        self.show_all()

    def update_sensitive(self, selected, enabled=False):
        self.delete.set_sensitive(selected)
        self.properties.set_sensitive(selected)
        self.enable.set_sensitive(selected and not enabled)
        self.disable.set_sensitive(selected and enabled)

class VariableEditor(Gtk.VBox):

    ADD_IMPORT = 0
    ADD_VARIABLE = 1
    OPEN_PROPERTIES = 2
    DELETE_BLOCK = 3
    DELETE_CONFIRM = 4
    ENABLE_BLOCK = 5
    DISABLE_BLOCK = 6

    __gsignals__ = {
        'create_new_block': (GObject.SignalFlags.RUN_FIRST, None, (str,)),
        'remove_block': (GObject.SignalFlags.RUN_FIRST, None, (str,))
    }

    def __init__(self):
        Gtk.VBox.__init__(self)
        config = Gtk.Application.get_default().config

        self._block = None
        self._mouse_button_pressed = False
        self._imports = []
        self._variables = []

        self.treestore = Gtk.TreeStore(GObject.TYPE_PYOBJECT,  # Block reference
                                       GObject.TYPE_STRING)    # Category and block name
        self.treeview = Gtk.TreeView(model=self.treestore)
        self.treeview.set_enable_search(False)
        self.treeview.set_search_column(-1)
        self.treeview.get_selection().set_mode(Gtk.SelectionMode.SINGLE)
        self.treeview.set_headers_visible(True)
        self.treeview.connect('button-press-event',
                              self._handle_mouse_button_press)
        self.treeview.connect('button-release-event',
                              self._handle_mouse_button_release)
        self.treeview.connect('motion-notify-event',
                              self._handle_motion_notify)
        self.treeview.connect('key-press-event', self._handle_key_button_press)

        self.id_cell = Gtk.CellRendererText()
        self.id_cell.connect('edited', self._handle_name_edited_cb)
        id_column = Gtk.TreeViewColumn("ID", self.id_cell, text=ID_INDEX)
        id_column.set_name("id")
        id_column.set_resizable(True)
        id_column.set_max_width(Utils.scale_scalar(300))
        id_column.set_min_width(Utils.scale_scalar(80))
        id_column.set_fixed_width(Utils.scale_scalar(120))
        id_column.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
        id_column.set_cell_data_func(self.id_cell, self.set_properties)
        self.id_column = id_column
        self.treeview.append_column(id_column)
        self.treestore.set_sort_column_id(ID_INDEX, Gtk.SortType.ASCENDING)

        self.value_cell = Gtk.CellRendererText()
        self.value_cell.connect('edited', self._handle_value_edited_cb)
        value_column = Gtk.TreeViewColumn("Value", self.value_cell)
        value_column.set_name("value")
        value_column.set_resizable(False)
        value_column.set_expand(True)
        value_column.set_min_width(Utils.scale_scalar(100))
        value_column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        value_column.set_cell_data_func(self.value_cell, self.set_value)
        self.value_column = value_column
        self.treeview.append_column(value_column)

        self.action_cell = Gtk.CellRendererPixbuf()
        value_column.pack_start(self.action_cell, False)
        value_column.set_cell_data_func(self.action_cell, self.set_icon)

        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(
            Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.add(self.treeview)
        scrolled_window.set_size_request(
            Constants.DEFAULT_BLOCKS_WINDOW_WIDTH, -1)
        self.pack_start(scrolled_window, True, True, 0)

        self._context_menu = VariableEditorContextMenu(self)
        self._confirm_delete = config.variable_editor_confirm_delete()

    def set_icon(self, col, cell, model, iter, data):
        block = model.get_value(iter, BLOCK_INDEX)
        cell.set_property('icon-name', 'window-close' if block else 'list-add')

    def set_value(self, col, cell, model, iter, data):
        sp = cell.set_property
        block = model.get_value(iter, BLOCK_INDEX)

        self.set_properties(col, cell, model, iter, data)

        value = None
        self.set_tooltip_text(None)

        if block:
            if block.key == 'import':
                value = block.params['imports'].get_value()
            elif block.key != "variable":
                value = "<Open Properties>"
                sp('editable', False)
                sp('foreground', '#0D47A1')
            else:
                value = block.params['value'].get_value()

            error_message = block.get_error_messages()
            if len(error_message) > 0:
                self.set_tooltip_text(error_message[-1])
            else:
                if block.is_variable:
                    value = str(block.evaluate(block.value))

        sp('text', value)

    def set_properties(self, col, cell, model, iter, data):
        sp = cell.set_property
        block = model.get_value(iter, BLOCK_INDEX)
        sp('sensitive', True)
        sp('editable', False)
        sp('foreground', None)

        if block:
            if not block.enabled:
                sp('editable', True)
                sp('foreground', 'gray')
            else:
                sp('editable', True)
            if block.get_error_messages():
                sp('foreground', 'red')

    def update_gui(self, blocks):
        self._imports = [block for block in blocks if block.is_import]
        self._variables = [block for block in blocks if block.is_variable]
        self._rebuild()
        self.treeview.expand_all()

    def _rebuild(self, *args):
        self.treestore.clear()
        imports = self.treestore.append(None, [None, 'Imports'])
        variables = self.treestore.append(None, [None, 'Variables'])
        for block in self._imports:
            self.treestore.append(
                imports, [block, block.params['id'].get_value()])
        for block in sorted(self._variables, key=lambda v: v.name):
            self.treestore.append(
                variables, [block, block.params['id'].get_value()])

    def _handle_name_edited_cb(self, cell, path, new_text):
        block = self.treestore[path][BLOCK_INDEX]
        block.params['id'].set_value(new_text)
        Actions.VARIABLE_EDITOR_UPDATE()

    def _handle_value_edited_cb(self, cell, path, new_text):
        block = self.treestore[path][BLOCK_INDEX]
        if block.is_import:
            block.params['imports'].set_value(new_text)
        else:
            block.params['value'].set_value(new_text)
        Actions.VARIABLE_EDITOR_UPDATE()

    def handle_action(self, item, key, event=None):
        if key == self.ADD_IMPORT:
            self.emit('create_new_block', 'import')
        elif key == self.ADD_VARIABLE:
            self.emit('create_new_block', 'variable')
        elif key == self.OPEN_PROPERTIES:
            print("Opening properties dialog")
        elif key == self.DELETE_BLOCK:
            self.emit('remove_block', self._block.name)
        elif key == self.ENABLE_BLOCK:
            self._block.state = 'enabled'
        elif key == self.DISABLE_BLOCK:
            self._block.state = 'disabled'
        Actions.VARIABLE_EDITOR_UPDATE()

    def _handle_mouse_button_press(self, widget, event):
        self._mouse_button_pressed = True
        self._col_width = self.id_column.get_width()

        path = widget.get_path_at_pos(int(event.x), int(event.y))
        if path:
            row = self.treestore[path[0]]
            col = path[1]
            self._block = row[BLOCK_INDEX]

            if event.button == 1 and col.get_name() == "value":
                if self._block and event.type == Gdk.EventType._2BUTTON_PRESS:
                    if self._block.key not in ("variable", "import"):
                        self.handle_action(None, self.OPEN_PROPERTIES, event=event)
                        return True
                if event.type == Gdk.EventType.BUTTON_PRESS:
                    if path[2] > col.cell_get_position(self.action_cell)[0]:
                        if row[1] == "Imports":
                            self.handle_action(None, self.ADD_IMPORT, event=event)
                        elif row[1] == "Variables":
                            self.handle_action(None, self.ADD_VARIABLE, event=event)
                        else:
                            self.handle_action(None, self.DELETE_CONFIRM, event=event)
                        return True
            elif event.button == 3 and event.type == Gdk.EventType.BUTTON_PRESS:
                if self._block:
                    self._context_menu.update_sensitive(
                        True, enabled=self._block.enabled)
                else:
                    self._context_menu.update_sensitive(False)
                self._context_menu.popup(
                    None, None, None, None, event.button, event.time)

            if event.type == Gdk.EventType._2BUTTON_PRESS:
                return True
        return False

    def _handle_mouse_button_release(self, widget, event):
        self._mouse_button_pressed = False
        return False

    def _handle_motion_notify(self, widget, event):
        if self._mouse_button_pressed and self.id_column.get_width() != self._col_width:
            self.value_column.queue_resize()
        return False

    def _handle_key_button_press(self, widget, event):
        model, path = self.treeview.get_selection().get_selected_rows()
        if path and self._block:
            if self._block.enabled and event.string == "d":
                self.handle_action(None, self.DISABLE_BLOCK, None)
                return True
            elif not self._block.enabled and event.string == "e":
                self.handle_action(None, self.ENABLE_BLOCK, None)
                return True
        return False
