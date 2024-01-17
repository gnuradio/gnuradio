import os

from ..core.Config import Config as CoreConfig


class Config(CoreConfig):

    name = 'GNU Radio Companion'

    gui_prefs_file = os.environ.get(
        'GRC_QT_PREFS_PATH', os.path.expanduser('~/.gnuradio/grc_qt.conf'))

    def __init__(self, install_prefix, *args, **kwargs):
        CoreConfig.__init__(self, *args, **kwargs)
        self.install_prefix = install_prefix

    @property
    def wiki_block_docs_url_prefix(self):
        return self._gr_prefs.get_string('grc-docs', 'wiki_block_docs_url_prefix', '')