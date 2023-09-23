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
    '''
    Defines the signals available from a running worker thread.

    Supported signals are:

    finished
        No data

    error
        tuple (exctype, value, traceback.format_exc() )

    result
        object data returned from processing, anything

    progress
        int indicating % progress

    '''
    finished = QtCore.Signal()
    error = QtCore.Signal(tuple)
    result = QtCore.Signal(object)
    progress = QtCore.Signal(int)


class Worker(QtCore.QRunnable):
    '''
    Worker thread

    Inherits from QRunnable to handler worker thread setup, signals and wrap-up.

    :param callback: The function callback to run on this worker thread. Supplied args and
                     kwargs will be passed through to the runner.
    :type callback: function
    :param args: Arguments to pass to the callback function
    :param kwargs: Keywords to pass to the callback function

    '''

    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()

        # Store constructor arguments (re-used for processing)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()

        # Add the callback to our kwargs
        self.kwargs['progress_callback'] = self.signals.progress

    @QtCore.Slot()
    def run(self):
        '''
        Initialise the runner function with passed args, kwargs.
        '''

        # Retrieve args/kwargs here; and fire processing using them
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            print("err")
            traceback.print_exc()
        else:
            self.signals.result.emit(result)  # Return the result of the processing
        finally:
            self.signals.finished.emit()  # Done



class ExampleBrowser(QtWidgets.QDialog, base.Component):
    # TODO: Use a model and modelitems
    file_to_open = QtCore.Signal(str)

    def __init__(self):
        super().__init__()
        uic.loadUi("grc/gui_qt/components/example_browser.ui", self)


        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Examples")

        

        self.examples = self.app.platform.examples
        self.modules = []
        self.current_files = []
        self.current_module = ""

        self.left_list.currentItemChanged.connect(self.populate_mid_list)
        self.mid_list.currentItemChanged.connect(self.populate_preview)

        self.open_button.clicked.connect(self.open_file)
        self.close_button.clicked.connect(self.reject)
    
    def populate(self, examples):
        self.examples = examples
        self.populate_left_list()
        self.left_list.setCurrentRow(0)
        self.mid_list.setCurrentRow(0)

    def populate_left_list(self):
        for ex in self.examples:
            if ex[0] not in self.modules:
                self.left_list.addItem(ex[0])
                self.modules.append(ex[0])

    def populate_mid_list(self):
        self.current_files = []
        self.mid_list.clear() # This triggers populate_preview, unfortunately
        for ex in self.examples:
            if ex[0] == self.left_list.currentItem().text():
                self.mid_list.addItem(ex[-1].split("/")[-1])
                self.current_files.append(ex)
        self.mid_list.setCurrentRow(0)
    
    def populate_preview(self):
        if self.mid_list.currentRow() == -1:
            return # we are currently clearing mid_list, wait
            
        current_example = None
        for ex in self.examples:
            if self.mid_list.currentItem().text() == ex[-1].split("/")[-1]:
                current_example = ex
                break
        self.title_label.setText(current_example[1])
        self.desc_label.setText(current_example[2])
        self.author_label.setText(current_example[3])

    def open_file(self):
        for ex in self.examples:
            if self.mid_list.currentItem().text() == ex[-1].split("/")[-1]:
                self.file_to_open.emit(ex[-1])
        self.done(0)
        