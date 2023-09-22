# Copyright 2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import json
import logging
import os
import time

from .io import yaml

logger = logging.getLogger(__name__)


class Cache(object):

    def __init__(self, filename, version=None):
        self.cache_file = filename
        self.version = version
        self.cache = {}
        self._cachetime = None
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
            self.need_cache_write = False
            logger.debug(f"Loading block cache from: {self.cache_file}")
            with open(self.cache_file, encoding='utf-8') as cache_file:
                cache = json.load(cache_file)
            cacheversion = cache.get("version", None)
            logger.debug(f"Cache version {cacheversion}")
            self._cachetime = cache.get("cached-at", 0)
            if cacheversion == self.version:
                logger.debug("Loaded block cache")
                self.cache = cache["cache"]
            else:
                logger.info(f"Outdated cache {self.cache_file} found, "
                            "will be overwritten.")
                raise ValueError()
        except (IOError, ValueError):
            self.need_cache_write = True

    def get_or_load(self, filename):
        self._accessed_items.add(filename)
        modtime = os.path.getmtime(filename)
        if modtime <= self._converter_mtime:
            try:
                cached = self.cache[filename]
                if int(cached["cached-at"] + 0.5) >= modtime:
                    return cached["data"]
                logger.info(f"Cache for {filename} outdated, loading yaml")
            except KeyError:
                pass

        with open(filename, encoding='utf-8') as fp:
            data = yaml.safe_load(fp)
        self.cache[filename] = {
            "cached-at": int(time.time()),
            "data": data
        }
        self.need_cache_write = True
        return data

    def save(self):
        if not self.need_cache_write:
            return

        logger.debug('Saving %d entries to json cache', len(self.cache))
        # Dumping to binary file is only supported for Python3 >= 3.6
        with open(self.cache_file, 'w', encoding='utf8') as cache_file:
            cache_content = {
                "version": self.version,
                "cached-at": self._cachetime,
                "cache": self.cache
            }
            cache_file.write(
                json.dumps(cache_content, ensure_ascii=False))

    def prune(self):
        for filename in (set(self.cache) - self._accessed_items):
            del self.cache[filename]

    def __enter__(self):
        self.load()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.save()
