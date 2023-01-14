from __future__ import absolute_import, print_function

# Standard modules
import logging
import os
import sys
import subprocess

# Third-party  modules
import six

from qtpy import QtCore, QtGui, QtWidgets
from qtpy.QtCore import Qt
from qtpy.QtGui import QStandardItemModel


# Logging
log = logging.getLogger(__name__)

#TODO: Move this to a separate file
class PreferencesDialog(QtWidgets.QDialog):
    def __init__(self):
        super().__init__()
        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Preferences")
        self.tabs = QtWidgets.QTabWidget()
        self.tabs.addTab(QtWidgets.QWidget(), "General")
        self.tabs.addTab(QtWidgets.QWidget(), "Flowgraphs")
        self.tabs.addTab(QtWidgets.QWidget(), "Appearance")        

        buttons = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.tabs)
        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)