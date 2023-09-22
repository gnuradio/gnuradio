"""Copyright 2021 The GNU Radio Contributors
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later
"""


import os
from os.path import expanduser, normpath, expandvars, exists
from collections import OrderedDict

from . import Constants


class Config(object):
    name = 'GNU Radio Companion (no gui)'
    license = __doc__.strip()
    website = 'https://www.gnuradio.org/'

    hier_block_lib_dir = os.environ.get(
        'GRC_HIER_PATH', Constants.DEFAULT_HIER_BLOCK_LIB_DIR)

    def __init__(self, version, version_parts=None, name=None, prefs=None):
        self._gr_prefs = prefs if prefs else DummyPrefs()
        self.version = version
        self.version_parts = version_parts or version[1:].split(
            '-', 1)[0].split('.')[:3]
        self.enabled_components = self._gr_prefs.get_string(
            'grc', 'enabled_components', '')
        if name:
            self.name = name

    @property
    def block_paths(self):
        paths_sources = (
            self.hier_block_lib_dir,
            os.environ.get('GRC_BLOCKS_PATH', ''),
            self._gr_prefs.get_string('grc', 'local_blocks_path', ''),
            self._gr_prefs.get_string('grc', 'global_blocks_path', ''),
        )

        collected_paths = sum((paths.split(os.pathsep)
                               for paths in paths_sources), [])

        valid_paths = [normpath(expanduser(expandvars(path)))
                       for path in collected_paths if exists(path)]
        # Deduplicate paths to avoid warnings about finding blocks twice, but
        # preserve order of paths
        valid_paths = list(OrderedDict.fromkeys(valid_paths))

        return valid_paths

    @property
    def default_flow_graph(self):
        user_default = (
            os.environ.get('GRC_DEFAULT_FLOW_GRAPH') or
            self._gr_prefs.get_string('grc', 'default_flow_graph', '') or
            os.path.join(self.hier_block_lib_dir, 'default_flow_graph.grc')
        )
        return user_default if exists(user_default) else Constants.DEFAULT_FLOW_GRAPH


class DummyPrefs(object):

    def get_string(self, category, item, default):
        return str(default)

    def set_string(self, category, item, value):
        pass

    def get_long(self, category, item, default):
        return int(default)

    def save(self):
        pass
