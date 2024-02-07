#TODO: This file is a modified copy of the old gui/Platform.py
"""
Copyright 2008,2013 Free Software Foundation, Inc.
This file is part of GNU Radio
SPDX-License-Identifier: GPL-2.0-or-later
"""


from qtpy import QtGui

from ... import Constants


def get_color(rgb):
    color = QtGui.QColor(rgb)
    return color


#################################################################################
# fg colors
#################################################################################
HIGHLIGHT_COLOR = get_color('#00FFFF')
BORDER_COLOR = get_color('#616161')
BORDER_COLOR_DISABLED = get_color('#888888')
FONT_COLOR = get_color('#000000')

# Deprecated blocks
# a light warm yellow
BLOCK_DEPRECATED_BACKGROUND_COLOR = get_color('#FED86B')
# orange
BLOCK_DEPRECATED_BORDER_COLOR = get_color('#FF540B')

# Block color constants
BLOCK_ENABLED_COLOR = get_color('#F1ECFF')
BLOCK_DISABLED_COLOR = get_color('#CCCCCC')
BLOCK_BYPASSED_COLOR = get_color('#F4FF81')

CONNECTION_ENABLED_COLOR = get_color('#616161')
CONNECTION_DISABLED_COLOR = get_color('#BBBBBB')
CONNECTION_ERROR_COLOR = get_color('#FF0000')

DARK_FLOWGRAPH_BACKGROUND_COLOR = get_color('#19232D')
LIGHT_FLOWGRAPH_BACKGROUND_COLOR = get_color('#FFFFFF')


#################################################################################
# port colors
#################################################################################
PORT_TYPE_TO_COLOR = {key: get_color(color) for name, key, sizeof, color in Constants.CORE_TYPES}
PORT_TYPE_TO_COLOR.update((key, get_color(color)) for key, (_, color) in Constants.ALIAS_TYPES.items())
