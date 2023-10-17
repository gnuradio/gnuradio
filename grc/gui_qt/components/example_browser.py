from __future__ import absolute_import, print_function

# Standard modules
import logging
import os
import sys
import subprocess
import yaml
import traceback

# Third-party  modules
import six

from qtpy import QtCore, QtGui, QtWidgets, uic
from qtpy.QtCore import Qt, QSettings
from qtpy.QtGui import QStandardItemModel

from .. import base
from ..properties import Paths


# Logging
log = logging.getLogger(__name__)

class WorkerSignals(QtCore.QObject):
    error = QtCore.Signal(tuple)
    result = QtCore.Signal(object)
    progress = QtCore.Signal(int)


class Worker(QtCore.QRunnable):
    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()

        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()

        self.kwargs['progress_callback'] = self.signals.progress

    @QtCore.Slot()
    def run(self):
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            print("Error in background task:")
            traceback.print_exc()
        else:
            self.signals.result.emit(result)



class ExampleBrowser(QtWidgets.QDialog, base.Component):
    # TODO: Use a model and modelitems
    file_to_open = QtCore.Signal(str)
    data_role = QtCore.Qt.UserRole

    def __init__(self):
        super().__init__()
        uic.loadUi(Paths.RESOURCES + "/example_browser.ui", self)

        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Examples")

        self.examples = self.app.platform.examples
        self.modules = []
        self.current_module = ""

        self.left_list.currentItemChanged.connect(self.update_mid_list)
        self.mid_list.currentItemChanged.connect(self.populate_preview)
        self.mid_list.itemDoubleClicked.connect(self.open_file)

        self.open_button.clicked.connect(self.open_file)
        self.cancel_button.clicked.connect(self.reject)

    def populate(self, examples):
        self.examples = examples

        for ex in self.examples:
            if ex["module"] not in self.modules:
                self.modules.append(ex["module"])
                self.left_list.addItem(ex["module"])
            item = QtWidgets.QListWidgetItem()
            item.setText(ex["name"])
            item.setData(self.data_role, QtCore.QVariant(ex))
            self.mid_list.addItem(item)

        self.left_list.setCurrentRow(0)
        self.update_mid_list()

    def update_mid_list(self):
        first = False
        for i in range(self.mid_list.count()):
            item = self.mid_list.item(i)

            if item.data(self.data_role)["module"] == self.left_list.currentItem().text():
                if not first:
                    self.mid_list.setCurrentRow(i)
                    first = True
                item.setHidden(False)
            else:
                item.setHidden(True)


    def populate_preview(self):
        ex = self.mid_list.currentItem().data(self.data_role)

        self.title_label.setText(ex["title"])
        self.desc_label.setText(ex["desc"])
        self.author_label.setText(ex["author"])

    def open_file(self):
        ex = self.mid_list.currentItem().data(self.data_role)

        self.file_to_open.emit(ex["path"])
        self.done(0)
