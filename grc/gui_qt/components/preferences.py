from __future__ import absolute_import, print_function

# Standard modules
import logging
import yaml

from qtpy.QtCore import Qt
from qtpy.QtWidgets import (QLineEdit, QTabWidget, QDialog,
                            QScrollArea, QVBoxLayout, QCheckBox,
                            QComboBox, QHBoxLayout, QDialogButtonBox,
                            QLabel)

from ..properties import Paths

# Logging
log = logging.getLogger(__name__)


class PreferencesDialog(QDialog):
    pref_dict = {}

    def __init__(self, qsettings):
        super().__init__()
        self.qsettings = qsettings

        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Preferences")
        self.tabs = QTabWidget()

        log.debug(f'Opening available preferences YAML: {Paths.AVAILABLE_PREFS_YML}')

        with open(Paths.AVAILABLE_PREFS_YML) as available_prefs_yml:
            self.pref_dict = yaml.safe_load(available_prefs_yml)

        for cat in self.pref_dict['categories']:
            cat['_scrollarea'] = QScrollArea()
            cat['_layout'] = QVBoxLayout()
            cat['_layout'].setAlignment(Qt.AlignTop)
            for item in cat['items']:
                full_key = cat['key'] + '/' + item['key']

                item['_label'] = QLabel(item['name'])

                if item['dtype'] == 'bool':
                    item['_edit'] = QCheckBox()

                    if self.qsettings.contains(full_key):
                        value = self.qsettings.value(full_key, False, type=bool)
                        item['_edit'].setChecked(value)
                    else:
                        item['_edit'].setChecked(item['default'])
                        self.qsettings.setValue(full_key, item['default'])

                elif item['dtype'] == 'enum':
                    item['_edit'] = QComboBox()
                    for opt in item['option_labels']:
                        item['_edit'].addItem(opt)
                    index = item['options'].index(self.qsettings.value(full_key, item['default'], type=str))
                    item['_edit'].setCurrentIndex(index)
                else:
                    if self.qsettings.contains(full_key):
                        item['_edit'] = QLineEdit(self.qsettings.value(full_key))
                    else:
                        item['_edit'] = QLineEdit(str(item['default']))
                        self.qsettings.setValue(full_key, item['default'])

                item['_line'] = QHBoxLayout()

                if 'tooltip' in item.keys():
                    item['_label'].setToolTip(item['tooltip'])
                    item['_edit'].setToolTip(item['tooltip'])

                item['_line'].addWidget(item['_label'])
                item['_line'].addWidget(item['_edit'])
                # This needs some work
                item['_line'].setStretch(0, 3)
                item['_line'].setStretch(1, 1)
                cat['_layout'].addLayout(item['_line'])

            cat['_scrollarea'].setLayout(cat['_layout'])
            self.tabs.addTab(cat['_scrollarea'], cat['name'])

        buttons = QDialogButtonBox.Ok | QDialogButtonBox.Cancel
        self.buttonBox = QDialogButtonBox(buttons)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.tabs)
        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)

    def save_all(self):
        log.debug(f'Writing changes to {self.qsettings.fileName()}')

        for cat in self.pref_dict['categories']:
            cat['_scrollarea'] = QScrollArea()
            cat['_layout'] = QVBoxLayout()
            cat['_layout'].setAlignment(Qt.AlignTop)
            for item in cat['items']:
                full_key = cat['key'] + '/' + item['key']

                if item['dtype'] == 'bool':
                    self.qsettings.setValue(full_key, item['_edit'].isChecked())
                elif item['dtype'] == 'enum':
                    self.qsettings.setValue(full_key, item['options'][item['_edit'].currentIndex()])
                else:
                    self.qsettings.setValue(full_key, item['_edit'].text())

        self.qsettings.sync()
