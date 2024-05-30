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
    def string(self, key: str, default: str = None):
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


class SCMMode(StrEnum):
    yes = auto()
    no = auto()
    auto = auto()


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
    scm_mode: SCMMode = None
    update_hash_only: bool = None  # False
    # todo: version should really be something else than a string
    version: str = None
    yes: bool = None

    def _load_prefs(self):
        if self.lang is None:
            self.lang = _prefs.string("lang")
            if self.lang:
                self.lang = Lang(self.lang)
        if self.scm_mode is None:
            self.scm_mode = _prefs.string("scm_mode")
            if self.scm_mode:
                self.scm_mode = SCMMode(self.scm_mode)
        if self.copyrightholder is None:
            config_copyright = _prefs.string("copyrightholder")
            self.copyrightholder = config_copyright

    def __post_init__(self):
        """
        This function is run at the end of the automatic __init__.
        Set all the things that we can set to defaults based on "yes" or "no" or "batch" here.
        Load settigs that have configuration variables.
        """
        if self.batch is None:
            self.batch = False
        elif self.batch:
            self.no = not self.yes
            if self.arglist is None:
                self.arglist = ""

        if self.no and self.yes:
            raise ValueError("Can't enforce both 'yes' and 'no' at the same time")

        if self.no:
            if self.complete is None:
                self.complete = False
            if self.is_component is None:
                self.is_component = False
            if self.scm_mode is None:
                self.scm_mode = SCMMode.no
            if self.update_hash_only is None:
                self.update_hash_only = False

        if self.yes:
            if self.complete is None:
                self.complete = True
            if self.is_component is None:
                self.is_component = True
            if self.scm_mode is None:
                self.scm_mode = SCMMode.yes
            if self.update_hash_only is None:
                self.update_hash_only = True

        self._load_prefs()
