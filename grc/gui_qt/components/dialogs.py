from __future__ import absolute_import, print_function

# Standard modules
import logging

# Third-party  modules

from qtpy import QtCore, QtGui, QtWidgets
from ..Constants import MIN_DIALOG_HEIGHT


class ErrorsDialog(QtWidgets.QDialog):
    def __init__(self, flowgraph):
        super().__init__()
        self.flowgraph = flowgraph
        self.store = []
        self.setModal(True)
        self.resize(700, MIN_DIALOG_HEIGHT)
        self.setWindowTitle("Errors and Warnings")
        buttons = QtWidgets.QDialogButtonBox.Close
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.buttonBox.rejected.connect(self.reject) # close
        self.treeview = QtWidgets.QTreeView()
        self.model = QtGui.QStandardItemModel()
        self.treeview.setModel(self.model)
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.treeview)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)
        self.update()

    def update(self):
        # TODO: Make sure the columns are wide enough
        self.model = QtGui.QStandardItemModel()
        self.model.setHorizontalHeaderLabels(['Source', 'Aspect', 'Message'])
        for element, message in self.flowgraph.iter_error_messages():
            if element.is_block:
                src, aspect = QtGui.QStandardItem(element.name), QtGui.QStandardItem('')
            elif element.is_connection:
                src = QtGui.QStandardItem(element.source_block.name)
                aspect = QtGui.QStandardItem("Connection to '{}'".format(element.sink_block.name))
            elif element.is_port:
                src = QtGui.QStandardItem(element.parent_block.name)
                aspect = QtGui.QStandardItem("{} '{}'".format(
                    'Sink' if element.is_sink else 'Source', element.name))
            elif element.is_param:
                src = QtGui.QStandardItem(element.parent_block.name)
                aspect = QtGui.QStandardItem("Param '{}'".format(element.name))
            else:
                src = aspect = QtGui.QStandardItem('')
            self.model.appendRow([src, aspect, QtGui.QStandardItem(message)])
        self.treeview.setModel(self.model)