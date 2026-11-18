import os
from os.path import expanduser, normpath, expandvars, exists
from collections import OrderedDict

from ..core.Config import Config as CoreConfig
from .. import paths
from . import Constants
from PyQt6.QtCore import QSettings


class Config(CoreConfig):
    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get('GRC_QT_PREFS_PATH', paths.get_config_file_path('grc_qt.conf'))

    def __init__(self, *args, **kwargs):
        CoreConfig.__init__(self, *args, **kwargs)
        self.gui_prefs_file = os.environ.get('GRC_QT_PREFS_PATH', paths.get_config_file_path('grc_qt.conf'))
        self.qsettings = QSettings(self.gui_prefs_file, QSettings.Format.IniFormat)
        Constants.DPI_SCALING = self.dpi_scaling
        Constants.update_font_size(self.font_size)

    @property
    def font_size(self):
        try:
            font_size = self._gr_prefs.get_long('grc', 'canvas_font_size',
                                                Constants.DEFAULT_FONT_SIZE)
            if font_size <= 0:
                raise ValueError
        except (ValueError, TypeError):
            font_size = Constants.DEFAULT_FONT_SIZE
        return font_size

    @property
    def dpi_scaling(self):
        try:
            dpi_scaling = self.qsettings.value('appearance/dpi_scaling', None, type=float)
            if dpi_scaling is None or dpi_scaling <= 0:
                if hasattr(self._gr_prefs, 'get_double'):
                    dpi_scaling = self._gr_prefs.get_double('grc', 'dpi_scaling', 1.0)
                else:
                    dpi_scaling = float(self._gr_prefs.get_string('grc', 'dpi_scaling', 1.0))
            if dpi_scaling <= 0:
                raise ValueError
        except (ValueError, TypeError, AttributeError):
            dpi_scaling = 1.0
        return dpi_scaling

    @property
    def wiki_block_docs_url_prefix(self):
        return self._gr_prefs.get_string('grc-docs', 'wiki_block_docs_url_prefix', '')
