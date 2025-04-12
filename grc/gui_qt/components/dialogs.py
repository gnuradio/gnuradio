from __future__ import absolute_import, print_function

from copy import copy

from ..Constants import MIN_DIALOG_HEIGHT, DEFAULT_PARAM_TAB
from qtpy.QtCore import Qt
from qtpy.QtGui import QStandardItem, QStandardItemModel
from qtpy.QtWidgets import (QLineEdit, QDialog, QDialogButtonBox, QTreeView,
                            QVBoxLayout, QTabWidget, QGridLayout, QWidget, QLabel,
                            QPushButton, QListWidget, QComboBox, QPlainTextEdit, QHBoxLayout,
                            QFileDialog, QApplication, QScrollArea)


class ErrorsDialog(QDialog):
    def __init__(self, flowgraph):
        super().__init__()
        self.flowgraph = flowgraph
        self.store = []
        self.setModal(True)
        self.resize(700, MIN_DIALOG_HEIGHT)
        self.setWindowTitle("Errors and Warnings")
        buttons = QDialogButtonBox.Close
        self.buttonBox = QDialogButtonBox(buttons)
        self.buttonBox.rejected.connect(self.reject)  # close
        self.treeview = QTreeView()
        self.treeview.setEditTriggers(QTreeView.NoEditTriggers)
        self.model = QStandardItemModel()
        self.treeview.setModel(self.model)
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.treeview)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)
        self.update()

    def update(self):
        # TODO: Make sure the columns are wide enough
        self.model.clear()
        self.model.setHorizontalHeaderLabels(['Source', 'Aspect', 'Message'])
        for element, message in self.flowgraph.iter_error_messages():
            if element.is_block:
                src, aspect = QStandardItem(element.name), QStandardItem('')
            elif element.is_connection:
                src = QStandardItem(element.source_block.name)
                aspect = QStandardItem("Connection to '{}'".format(element.sink_block.name))
            elif element.is_port:
                src = QStandardItem(element.parent_block.name)
                aspect = QStandardItem("{} '{}'".format(
                    'Sink' if element.is_sink else 'Source', element.name))
            elif element.is_param:
                src = QStandardItem(element.parent_block.name)
                aspect = QStandardItem("Param '{}'".format(element.name))
            else:
                src = aspect = QStandardItem('')
            self.model.appendRow([src, aspect, QStandardItem(message)])


class PropsDialog(QDialog):
    def __init__(self, parent_block, force_show_id):
        super().__init__()
        self.setMinimumSize(700, MIN_DIALOG_HEIGHT)
        self._block = parent_block
        self.qsettings = QApplication.instance().qsettings
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

        self.tabs = QTabWidget()
        self.ignore_dtype_labels = ["_multiline", "_multiline_python_external", "file_open", "file_save"]

        for cat in unique_categories():
            qvb = self.build_param_entrys(cat)
            tab = QWidget()
            tab.setLayout(qvb)
            scrollarea = QScrollArea()
            scrollarea.setWidget(tab)
            self.tabs.addTab(scrollarea, cat)

        self.scroll_error = QScrollArea()

        # Add example tab
        self.example_tab = QWidget()
        self.example_layout = QVBoxLayout()
        self.example_tab.setLayout(self.example_layout)
        self.example_list = QListWidget()
        self.example_list.itemDoubleClicked.connect(lambda ex: self.open_example(ex))

        buttons = QDialogButtonBox.Ok | QDialogButtonBox.Cancel
        self.buttonBox = QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.tabs)

        if self._block.enabled:
            self.scroll_error.hide()
            self.layout.addWidget(self.scroll_error)

            if self._block.get_error_messages():
                error_msg = QLabel()
                error_msg.setText('\n'.join(self._block.get_error_messages()))
                self.scroll_error.setWidget(error_msg)
                self.scroll_error.show()

        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)
        self._block.old_data = self._block.export_data()

    def build_param_entrys(self, cat):
        qvb = QGridLayout()
        qvb.setAlignment(Qt.AlignTop)
        qvb.setVerticalSpacing(5)
        qvb.setHorizontalSpacing(20)
        i = 0
        for param in self._block.params.values():
            if self.force_show_id and param.dtype == 'id':
                param.hide = 'none'
            if param.category == cat and param.hide != "all":
                dtype_label = None
                if param.dtype not in self.ignore_dtype_labels:
                    dtype_label = QLabel(f"[{param.dtype}]")
                qvb.addWidget(QLabel(param.name), i, 0)
                if param.dtype == "enum" or param.options:
                    dropdown = QComboBox()
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
                        dropdown.currentIndexChanged.connect(self.gui_update)
                    else:
                        dropdown.setEditable(True)
                        dropdown.setCurrentIndex(
                            dropdown.param_values.index(param.get_value())
                        )
                        dropdown.currentIndexChanged.connect(self.gui_update)
                elif param.dtype in ("file_open", "file_save"):
                    dtype_label = QPushButton("...")
                    dtype_label.setFlat(True)
                    file_name = QLineEdit(param.value)
                    file_name.param = param
                    qvb.addWidget(file_name, i, 1)
                    self.edit_params.append(file_name)
                    if param.dtype == "file_open":
                        dtype_label.clicked.connect(self.open_filero)
                    else:
                        dtype_label.clicked.connect(self.open_filerw)
                elif param.dtype == "_multiline":
                    line_edit = QPlainTextEdit(param.value)
                    line_edit.param = param
                    qvb.addWidget(line_edit, i, 1)
                    self.edit_params.append(line_edit)
                elif param.dtype == "_multiline_python_external":
                    ext_param = copy(param)

                    def open_editor(widget=None):
                        self._block.parent_flowgraph.gui.install_external_editor(
                            ext_param)

                    def open_chooser(widget=None):
                        self._block.parent_flowgraph.gui.remove_external_editor(param=ext_param)
                        editor, filtr = QFileDialog.getOpenFileName(
                            self,
                        )
                        self.qsettings.setValue("grc/editor", editor)
                    editor_widget = QWidget()
                    editor_widget.setLayout(QHBoxLayout())
                    open_editor_button = QPushButton("Open in Editor")
                    open_editor_button.clicked.connect(open_editor)
                    choose_editor_button = QPushButton("Choose Editor")
                    choose_editor_button.clicked.connect(open_chooser)
                    editor_widget.layout().addWidget(open_editor_button)
                    editor_widget.layout().addWidget(choose_editor_button)
                    line_edit = QPlainTextEdit(param.value)
                    line_edit.param = param
                    qvb.addWidget(editor_widget, i, 1)
                else:
                    line_edit = QLineEdit(param.value)
                    line_edit.returnPressed.connect(self.gui_update)
                    line_edit.param = param
                    qvb.addWidget(line_edit, i, 1)
                    self.edit_params.append(line_edit)
                if dtype_label:
                    qvb.addWidget(dtype_label, i, 2)
            i += 1
        return qvb

    def gui_update(self):
        index = self.tabs.currentIndex()
        self.tabs.currentWidget().widget().hide()
        to_delete = self.tabs.currentWidget().takeWidget()

        for par in self.edit_params:
            if isinstance(par, QLineEdit):
                par.param.set_value(par.text())
            elif isinstance(par, QPlainTextEdit):  # Multiline
                par.param.set_value(par.toPlainText())
            else:  # Dropdown/ComboBox
                for key, val in par.param.options.items():
                    if val == par.currentText():
                        par.param.set_value(key)
        self._block.rewrite()
        self._block.validate()
        cat = self.tabs.tabText(index)
        self.edit_params.clear()
        qvb = self.build_param_entrys(cat)
        tab = QWidget()
        tab.setLayout(qvb)
        to_delete.deleteLater()
        self.tabs.currentWidget().setWidget(tab)
        if self._block.enabled:
            if self._block.get_error_messages():
                error_msg = QLabel()
                error_msg.setText('\n'.join(self._block.get_error_messages()))
                if self.scroll_error.widget():
                    to_delete = self.scroll_error.takeWidget()
                    to_delete.deleteLater()
                self.scroll_error.setWidget(error_msg)
                self.scroll_error.show()
            else:
                if self.scroll_error.widget():
                    to_delete = self.scroll_error.takeWidget()
                    to_delete.deleteLater()
                self.scroll_error.hide()

        self._block.parent.gui.blockPropsChange.emit(self._block)

    def find_param_widget(self, button):
        # Find the correct layout
        layout = self.tabs.currentWidget().widget().layout()
        # Find the location in the layout of the clicked button
        location = layout.getItemPosition(layout.indexOf(button))
        # The required widget is at column 1
        return layout.itemAtPosition(location[0], 1).widget()

    def open_filero(self):
        f_name, fltr = QFileDialog.getOpenFileName(self, "Open a Data File")
        button = self.sender()
        self.find_param_widget(button).setText(f_name)

    def open_filerw(self):
        f_name, fltr = QFileDialog.getSaveFileName(self, "Save a Data File")
        button = self.sender()
        self.find_param_widget(button).setText(f_name)

    def accept(self):
        super().accept()
        for par in self.edit_params:
            if isinstance(par, QLineEdit):
                par.param.set_value(par.text())
            elif isinstance(par, QPlainTextEdit):  # Multiline
                par.param.set_value(par.toPlainText())
            else:  # Dropdown/ComboBox
                for key, val in par.param.options.items():
                    if val == par.currentText():
                        par.param.set_value(key)
        self._block.rewrite()
        self._block.validate()
        self._block.gui.create_shapes_and_labels()
        self._block.parent.gui.blockPropsChange.emit(self._block)

    def reject(self):
        try:
            name = self._block.old_data['name']
        except KeyError:
            name = self._block.old_data['parameters']['id']
        self._block.import_data(name, self._block.old_data['states'], self._block.old_data['parameters'])
        self._block.rewrite()
        self._block.validate()
        self._block.gui.create_shapes_and_labels()
        self._block.parent.gui.blockPropsChange.emit(self._block)
        super().reject()

    def open_example(self, ex=None):
        # example is None if the "Open examples" button was pushed
        if ex is None:
            ex = self.example_list.currentItem()
        self._block.parent.gui.app.MainWindow.open_example(ex.text())
        self.close()
