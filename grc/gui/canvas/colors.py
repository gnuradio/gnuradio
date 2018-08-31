"""
Copyright 2008,2013 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

from __future__ import absolute_import

from gi.repository import Gtk, Gdk, cairo
# import pycairo

from .. import Constants


def get_color(color_code):
    color = Gdk.RGBA()
    color.parse(color_code)
    return color.red, color.green, color.blue, color.alpha
    # chars_per_color = 2 if len(color_code) > 4 else 1
    # offsets = range(1, 3 * chars_per_color + 1, chars_per_color)
    # return tuple(int(color_code[o:o + 2], 16) / 255.0 for o in offsets)

#################################################################################
# fg colors
#################################################################################

HIGHLIGHT_COLOR = get_color('#00FFFF')
BORDER_COLOR = get_color('#616161')
BORDER_COLOR_DISABLED = get_color('#888888')
FONT_COLOR = get_color('#000000')

# Missing blocks stuff
MISSING_BLOCK_BACKGROUND_COLOR = get_color('#FFF2F2')
MISSING_BLOCK_BORDER_COLOR = get_color('#FF0000')

# Flow graph color constants
FLOWGRAPH_BACKGROUND_COLOR = get_color('#FFFFFF')
COMMENT_BACKGROUND_COLOR = get_color('#F3F3F3')
FLOWGRAPH_EDGE_COLOR = COMMENT_BACKGROUND_COLOR

# Block color constants
BLOCK_ENABLED_COLOR = get_color('#F1ECFF')
BLOCK_DISABLED_COLOR = get_color('#CCCCCC')
BLOCK_BYPASSED_COLOR = get_color('#F4FF81')

# Connection color constants
CONNECTION_ENABLED_COLOR = get_color('#000000')
CONNECTION_DISABLED_COLOR = get_color('#BBBBBB')
CONNECTION_ERROR_COLOR = get_color('#FF0000')

DEFAULT_DOMAIN_COLOR = get_color('#777777')


#################################################################################
# port colors
#################################################################################

PORT_TYPE_TO_COLOR = {key: get_color(color) for name, key, sizeof, color in Constants.CORE_TYPES}
PORT_TYPE_TO_COLOR.update((key, get_color(color)) for key, (_, color) in Constants.ALIAS_TYPES.items())


#################################################################################
# param box colors
#################################################################################

