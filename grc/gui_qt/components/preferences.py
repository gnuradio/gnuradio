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

from ..preferences_yml import pref_dict

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
        
        for cat in pref_dict['categories']:
            cat['_scrollarea'] = QtWidgets.QScrollArea()
            cat['_layout'] = QtWidgets.QVBoxLayout()
            cat['_layout'].setAlignment(Qt.AlignTop)
            for item in cat['items']:
                
                item['_label'] = QtWidgets.QLabel(item['name'])
                if item['dtype'] == 'bool':
                    item['_edit'] = QtWidgets.QCheckBox()
                    item['_edit'].setCheckState(item['default'])
                else:
                    item['_edit'] = QtWidgets.QLineEdit(str(item['default']))
                item['_line'] = QtWidgets.QHBoxLayout()

                item['_line'].addWidget(item['_label'])
                item['_line'].addWidget(item['_edit'])
                # This needs some work
                item['_line'].setStretch(0,3)
                item['_line'].setStretch(1,1)
                cat['_layout'].addLayout(item['_line'])

            cat['_scrollarea'].setLayout(cat['_layout'])
            self.tabs.addTab(cat['_scrollarea'], cat['name'])

        buttons = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        self.buttonBox = QtWidgets.QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.tabs)
        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)