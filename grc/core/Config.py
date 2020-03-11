"""Copyright 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from __future__ import absolute_import

import os
from os.path import expanduser, normpath, expandvars, exists

from . import Constants


class Config(object):
    name = 'GNU Radio Companion (no gui)'
    license = __doc__.strip()
    website = 'https://www.gnuradio.org/'

    hier_block_lib_dir = os.environ.get('GRC_HIER_PATH', Constants.DEFAULT_HIER_BLOCK_LIB_DIR)

    def __init__(self, version, version_parts=None, name=None, prefs=None):
        self._gr_prefs = prefs if prefs else DummyPrefs()
        self.version = version
        self.version_parts = version_parts or version[1:].split('-', 1)[0].split('.')[:3]
        if name:
            self.name = name

    @property
    def block_paths(self):
        path_list_sep = {'/': ':', '\\': ';'}[os.path.sep]

        paths_sources = (
            self.hier_block_lib_dir,
            os.environ.get('GRC_BLOCKS_PATH', ''),
            self._gr_prefs.get_string('grc', 'local_blocks_path', ''),
            self._gr_prefs.get_string('grc', 'global_blocks_path', ''),
        )

        collected_paths = sum((paths.split(path_list_sep)
                               for paths in paths_sources), [])

        valid_paths = [normpath(expanduser(expandvars(path)))
                       for path in collected_paths if exists(path)]

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
