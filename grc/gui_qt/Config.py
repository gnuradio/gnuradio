import os
from os.path import expanduser, normpath, expandvars, exists
from collections import OrderedDict

from ..core.Config import Config as CoreConfig
from qtpy import QtCore


class Config(CoreConfig):

    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get(
        'GRC_QT_PREFS_PATH', os.path.expanduser('~/.gnuradio/grc_qt.conf'))

    def __init__(self, install_prefix, *args, **kwargs):
        CoreConfig.__init__(self, *args, **kwargs)
        self.install_prefix = install_prefix
        self.qsettings = QtCore.QSettings(self.gui_prefs_file, QtCore.QSettings.IniFormat)

    @property
    def wiki_block_docs_url_prefix(self):
        return self._gr_prefs.get_string('grc-docs', 'wiki_block_docs_url_prefix', '')

    @property
    def block_paths(self):
        paths_sources = (
            self.hier_block_lib_dir,
            os.environ.get('GRC_BLOCKS_PATH', ''),
            self._gr_prefs.get_string('grc', 'local_blocks_path', ''),
            self._gr_prefs.get_string('grc', 'global_blocks_path', ''),
            self.qsettings.value('grc/custom_block_paths', ''),
        )

        collected_paths = sum((paths.split(os.pathsep)
                               for paths in paths_sources), [])

        valid_paths = [normpath(expanduser(expandvars(path)))
                       for path in collected_paths if exists(path)]
        # Deduplicate paths to avoid warnings about finding blocks twice, but
        # preserve order of paths
        valid_paths = list(OrderedDict.fromkeys(valid_paths))

        return valid_paths
