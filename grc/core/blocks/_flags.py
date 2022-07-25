# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


class Flags(object):

    THROTTLE = 'throttle'
    DISABLE_BYPASS = 'disable_bypass'
    NEED_QT_GUI = 'need_qt_gui'
    DEPRECATED = 'deprecated'
    NOT_DSP = 'not_dsp'
    SHOW_ID = 'show_id'
    HAS_PYTHON = 'python'
    HAS_CPP = 'cpp'

    def __init__(self, flags=None):
        if flags is None:
            flags = set()
        if isinstance(flags, str):
            flags = (f.strip() for f in flags.replace(',', '').split())
        self.data = set(flags)

    def __getattr__(self, item):
        return item in self

    def __contains__(self, item):
        return item in self.data

    def __str__(self):
        return ', '.join(self.data)

    def set(self, *flags):
        self.data.update(flags)
