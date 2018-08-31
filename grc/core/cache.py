# Copyright 2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import, print_function, unicode_literals

from io import open
import json
import logging
import os

import six

from .io import yaml

logger = logging.getLogger(__name__)


class Cache(object):

    def __init__(self, filename):
        self.cache_file = filename
        self.cache = {}
        self.need_cache_write = True
        self._accessed_items = set()
        try:
            os.makedirs(os.path.dirname(filename))
        except OSError:
            pass
        try:
            self._converter_mtime = os.path.getmtime(filename)
        except OSError:
            self._converter_mtime = -1

    def load(self):
        try:
            logger.debug("Loading block cache from: {}".format(self.cache_file))
            with open(self.cache_file, encoding='utf-8') as cache_file:
                self.cache = json.load(cache_file)
            self.need_cache_write = False
        except (IOError, ValueError):
            self.need_cache_write = True

    def get_or_load(self, filename):
        self._accessed_items.add(filename)
        if os.path.getmtime(filename) <= self._converter_mtime:
            try:
                return self.cache[filename]
            except KeyError:
                pass

        with open(filename, encoding='utf-8') as fp:
            data = yaml.safe_load(fp)
        self.cache[filename] = data
        self.need_cache_write = True
        return data

    def save(self):
        if not self.need_cache_write:
            return

        logger.info('Saving %d entries to json cache', len(self.cache))
        # Dumping to binary file is only supported for Python3 >= 3.6
        with open(self.cache_file, 'w', encoding='utf8') as cache_file:
            cache_file.write(json.dumps(self.cache, ensure_ascii=False))

    def prune(self):
        for filename in (set(self.cache) - self._accessed_items):
            del self.cache[filename]

    def __enter__(self):
        self.load()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.save()


def byteify(data):
    if isinstance(data, dict):
        return {byteify(key): byteify(value) for key, value in six.iteritems(data)}
    elif isinstance(data, list):
        return [byteify(element) for element in data]
    elif isinstance(data, six.text_type) and six.PY2:
        return data.encode('utf-8')
    else:
        return data
