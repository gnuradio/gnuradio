from __future__ import absolute_import, print_function

# Standard modules
import logging

# Third-party  modules
import six

from qtpy import QtCore, QtGui, QtWidgets


class ErrorsDialog(QtWidgets.QDialog):
    def __init__(self, flowgraph):
        super().__init__()
        self.flowgraph = flowgraph
        self.store = []
        self.setModal(True)
        self.setWindowTitle("Errors and Warnings")
        buttons = QtWidgets.QDialogButtonBox.Ok
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.treeview = QtWidgets.QTreeView()
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.treeview)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)
        
    
    def update(self):
        self.store = []
        for element, message in self.flowgraph.iter_error_messages():
            if element.is_block:
                src, aspect = element.name, ''
            elif element.is_connection:
                src = element.source_block.name
                aspect = "Connection to '{}'".format(element.sink_block.name)
            elif element.is_port:
                src = element.parent_block.name
                aspect = "{} '{}'".format(
                    'Sink' if element.is_sink else 'Source', element.name)
            elif element.is_param:
                src = element.parent_block.name
                aspect = "Param '{}'".format(element.name)
            else:
                src = aspect = ''
            self.store.append([src, aspect, message])


'''
class ErrorsDialog_(Gtk.Dialog):
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
                aspect = "{} '{}'".format(
                    'Sink' if element.is_sink else 'Source', element.name)
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
'''