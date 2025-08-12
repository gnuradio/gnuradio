import os
from os.path import expanduser, normpath, expandvars, exists, join
from collections import OrderedDict

import Constants
from ..core.Config import Config as CoreConfig
from ..main import get_config_file_path
from qtpy import QtCore


class Config(CoreConfig):
    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get('GRC_QT_PREFS_PATH', get_config_file_path('grc_qt.conf'))

    def __init__(self, *args, **kwargs):
        CoreConfig.__init__(self, *args, **kwargs)
        self.qsettings = QtCore.QSettings(self.gui_prefs_file, QtCore.QSettings.IniFormat)

    @property
    def wiki_block_docs_url_prefix(self):
        return self._gr_prefs.get_string('grc-docs', 'wiki_block_docs_url_prefix', '')
