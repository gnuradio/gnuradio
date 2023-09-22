# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from ._flags import Flags
from ._templates import MakoTemplates

from .block import Block

from ._build import build


build_ins = {}


def register_build_in(cls):
    cls.loaded_from = '(build-in)'
    build_ins[cls.key] = cls
    return cls


from .dummy import DummyBlock
from .embedded_python import EPyBlock, EPyModule
from .virtual import VirtualSink, VirtualSource
