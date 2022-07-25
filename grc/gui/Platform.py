"""
Copyright 2008, 2009 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import sys
import os

from .Config import Config
from . import canvas
from ..core.platform import Platform as CorePlatform
from ..core.utils.backports import ChainMap


class Platform(CorePlatform):

    def __init__(self, *args, **kwargs):
        CorePlatform.__init__(self, *args, **kwargs)

        # Ensure conf directories
        gui_prefs_file = self.config.gui_prefs_file
        if not os.path.exists(os.path.dirname(gui_prefs_file)):
            os.mkdir(os.path.dirname(gui_prefs_file))

        self._move_old_pref_file()

    def get_prefs_file(self):
        return self.config.gui_prefs_file

    def _move_old_pref_file(self):
        gui_prefs_file = self.config.gui_prefs_file
        old_gui_prefs_file = os.environ.get(
            'GRC_PREFS_PATH', os.path.expanduser('~/.grc'))
        if gui_prefs_file == old_gui_prefs_file:
            return  # prefs file overridden with env var
        if os.path.exists(old_gui_prefs_file) and not os.path.exists(gui_prefs_file):
            try:
                import shutil
                shutil.move(old_gui_prefs_file, gui_prefs_file)
            except Exception as e:
                print(e, file=sys.stderr)

    ##############################################
    # Factories
    ##############################################
    Config = Config
    FlowGraph = canvas.FlowGraph
    Connection = canvas.Connection

    def new_block_class(self, **data):
        cls = CorePlatform.new_block_class(self, **data)
        return canvas.Block.make_cls_with_base(cls)

    block_classes_build_in = {key: canvas.Block.make_cls_with_base(cls)
                              for key, cls in CorePlatform.block_classes_build_in.items()}
    block_classes = ChainMap({}, block_classes_build_in)

    port_classes = {key: canvas.Port.make_cls_with_base(cls)
                    for key, cls in CorePlatform.port_classes.items()}
    param_classes = {key: canvas.Param.make_cls_with_base(cls)
                     for key, cls in CorePlatform.param_classes.items()}
