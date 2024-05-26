#
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
from dataclasses import dataclass
from enum import auto
from ..tools import StrEnum
from typing import List

from gnuradio.gr import prefs


# Helpers


class _Prefs:
    """
    Private helper class to avoid having to type
        gr.prefs().get_something(section="modtool")
    for every configurable field
    """

    def __init__(self):
        self.prefs = prefs()

    # implement only the getters we need for now
    def string(self, key: str, default: str):
        if default is None:
            if not self.prefs.has_option("modtool", key):
                return None
            return self.prefs.get_string("modtool", key, "")
        return self.prefs.get_string("modtool", key, default)


_prefs = _Prefs()

# Enum classes
# Using StrEnum for the most part because we can stil == "somestrig" that way


class Lang(StrEnum):
    cpp = auto()
    python = auto()


class Blocktype(StrEnum):
    sink = auto()
    source = auto()
    sync = auto()
    decimator = auto()
    interpolator = auto()
    general = auto()
    tagged_stream = auto()
    hier = auto()
    noblock = auto()


# The main act:
# This is our default-configurable Tool Configuration


@dataclass
class ToolConfig:
    """
    Class to hold the configuration of the modtool instance
    """

    addl_includes: List[str] = ()
    arglist: List[str] = ()  # Argument list for add
    batch: bool = None  # False
    blockname: str = None
    blocktype: Blocktype = None
    complete: bool = None  # False
    copyrightholder: str = None
    define_symbols: List[str] = ()
    fullblockname: str = None
    include_blacklisted: bool = ()
    includedir: str = None
    is_component: bool = None  # False
    lang: Lang = None
    license: str = None
    modname: str = None
    newname: str = None
    no: bool = None  # False
    oldname: str = None
    pattern: str = None
    pydir: str = None
    update_hash_only: bool = None  # False
    # todo: version should really be something else than a string
    version: str = _f(None)
    yes: bool = _f(None)
