from __future__ import absolute_import, print_function

# Standard modules
import logging

# Third-party  modules

from qtpy import QtCore, QtGui, QtWidgets
from ..Constants import MIN_DIALOG_HEIGHT, DEFAULT_PARAM_TAB


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

class PropsDialog(QtWidgets.QDialog):
    def __init__(self, parent_block, force_show_id):
        super().__init__()
        self.setMinimumSize(600, 400)
        self._block = parent_block
        self.setModal(True)
        self.force_show_id = force_show_id

        self.setWindowTitle(f"Properties: {self._block.label}")

        categories = (p.category for p in self._block.params.values())

        def unique_categories():
            seen = {DEFAULT_PARAM_TAB}
            yield DEFAULT_PARAM_TAB
            for cat in categories:
                if cat in seen:
                    continue
                yield cat
                seen.add(cat)

        self.edit_params = []

        self.tabs = QtWidgets.QTabWidget()
        for cat in unique_categories():
            qvb = QtWidgets.QGridLayout()
            qvb.setAlignment(QtCore.Qt.AlignTop)
            qvb.setVerticalSpacing(5)
            qvb.setHorizontalSpacing(20)
            i = 0
            for param in self._block.params.values():
                if force_show_id and param.dtype == 'id':
                        param.hide = 'none'
                if param.category == cat and param.hide != "all":
                    qvb.addWidget(QtWidgets.QLabel(param.name), i, 0)
                    if param.dtype == "enum" or param.options:
                        dropdown = QtWidgets.QComboBox()
                        for opt in param.options.values():
                            dropdown.addItem(opt)
                        dropdown.param_values = list(param.options)
                        dropdown.param = param
                        qvb.addWidget(dropdown, i, 1)
                        self.edit_params.append(dropdown)
                        if param.dtype == "enum":
                            dropdown.setCurrentIndex(
                                dropdown.param_values.index(param.get_value())
                            )
                        else:
                            dropdown.setEditable(True)
                            value_label = (
                                param.options[param.value]
                                if param.value in param.options
                                else param.value
                            )
                            dropdown.setCurrentText(value_label)
                    else:
                        line_edit = QtWidgets.QLineEdit(param.value)
                        line_edit.param = param
                        qvb.addWidget(line_edit, i, 1)
                        self.edit_params.append(line_edit)
                    qvb.addWidget(QtWidgets.QLabel("TYPE"), i, 2)
                i += 1
            tab = QtWidgets.QWidget()
            tab.setLayout(qvb)
            self.tabs.addTab(tab, cat)

        # Add example tab
        ex_amount = 0
        self.example_tab = QtWidgets.QWidget()
        self.example_layout = QtWidgets.QVBoxLayout()
        self.example_tab.setLayout(self.example_layout)
        self.example_list = QtWidgets.QListWidget()
        self.example_list.itemDoubleClicked.connect(lambda ex: self.open_example(ex))
        try:
            examples = self._block.parent.app.BlockLibrary.get_examples(self._block.key)
            ex_amount = len(examples)
            self.example_list.addItems(examples)
            self.example_layout.addWidget(self.example_list)
            self.open_ex_button = QtWidgets.QPushButton("Open example")
            self.open_ex_button.clicked.connect(lambda: self.open_example())
            #policy = QtWidgets.QSizePolicy()
            #policy.setHorizontalPolicy(QtWidgets.QSizePolicy.Preferred)
            #self.open_ex_button.setSizePolicy(policy)
            self.example_layout.addWidget(self.open_ex_button, alignment=QtCore.Qt.AlignRight)
        except KeyError:
            self.example_layout.addWidget(QtWidgets.QLabel("No examples use this block"))

        self.tabs.addTab(self.example_tab, f"Examples ({ex_amount})")

        buttons = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.tabs)
        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)

    def accept(self):
        super().accept()
        self._block.oldData = self._block.export_data()
        for par in self.edit_params:
            if isinstance(par, QtWidgets.QLineEdit):
                par.param.set_value(par.text())
            else:  # Dropdown/ComboBox
                for key, val in par.param.options.items():
                    if val == par.currentText():
                        par.param.set_value(key)
        self._block.rewrite()
        self._block.validate()
        self._block.create_shapes_and_labels()
        self._block.parent.blockPropsChange.emit(self._block)

    def open_example(self, ex=None):
        # example is None if the "Open examples" button was pushed
        if ex is None:
            ex = self.example_list.currentItem()
        self._block.parent.app.MainWindow.open_example(ex.text())
        self.close()
