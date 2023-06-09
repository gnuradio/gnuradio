from __future__ import absolute_import, print_function

# Standard modules
import logging
import os
import sys
import subprocess
import yaml

# Third-party  modules
import six

from qtpy import QtCore, QtGui, QtWidgets
from qtpy.QtCore import Qt, QSettings
from qtpy.QtGui import QStandardItemModel

from ..properties import Paths

# Logging
log = logging.getLogger(__name__)

#TODO: Move this to a separate file
class PreferencesDialog(QtWidgets.QDialog):
    pref_dict = {}

    def __init__(self):
        super().__init__()

        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Preferences")
        self.tabs = QtWidgets.QTabWidget()

        self.prefs = QSettings('GNU_Radio', 'GNU_Radio_Companion')
        log.info(f'Opening settings file: {self.prefs.fileName()}')

        log.debug(f'Opening available preferences YAML: {Paths.AVAILABLE_PREFS_YML}')

        with open(Paths.AVAILABLE_PREFS_YML) as available_prefs_yml:
            self.pref_dict = yaml.safe_load(available_prefs_yml)

        for cat in self.pref_dict['categories']:
            cat['_scrollarea'] = QtWidgets.QScrollArea()
            cat['_layout'] = QtWidgets.QVBoxLayout()
            cat['_layout'].setAlignment(Qt.AlignTop)
            for item in cat['items']:
                key = item['key']

                item['_label'] = QtWidgets.QLabel(item['name'])

                if item['dtype'] == 'bool':
                    item['_edit'] = QtWidgets.QCheckBox()

                    if self.prefs.contains(key):
                        value = self.prefs.value(key)
                        if value == 'True':
                            item['_edit'].setChecked(True)
                        elif value == 'False':
                            item['_edit'].setChecked(False)
                        else:
                            log.warn(f'Invalid preferences value for {key}: {value}. Ignoring')
                            continue
                    else:
                        item['_edit'].setChecked(item['default'])
                        self.prefs.setValue(item['key'], item['default'])

                else: # TODO: Dropdowns

                    if self.prefs.contains(item['key']):
                        item['_edit'] = QtWidgets.QLineEdit(self.prefs.value(item['key']))
                    else:
                        item['_edit'] = QtWidgets.QLineEdit(str(item['default']))
                        self.prefs.setValue(item['key'], item['default'])

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

    def save_all(self):
        log.debug(f'Writing changes to {self.prefs.fileName()}')

        for cat in self.pref_dict['categories']:
            cat['_scrollarea'] = QtWidgets.QScrollArea()
            cat['_layout'] = QtWidgets.QVBoxLayout()
            cat['_layout'].setAlignment(Qt.AlignTop)
            for item in cat['items']:
                if item['dtype'] == 'bool':
                    self.prefs.setValue(item['key'], str(item['_edit'].isChecked()))
                else:
                    self.prefs.setValue(item['key'], item['_edit'].text())

        self.prefs.sync()
