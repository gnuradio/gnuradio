"""
Copyright 2008 Free Software Foundation, Inc.
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

import pygtk
pygtk.require('2.0')
import gtk

_COLORMAP = gtk.gdk.colormap_get_system() #create all of the colors
def get_color(color_code): return _COLORMAP.alloc_color(color_code, True, True)

HIGHLIGHT_COLOR = get_color('#00FFFF')
BORDER_COLOR = get_color('black')
#param entry boxes
PARAM_ENTRY_TEXT_COLOR = get_color('black')
ENTRYENUM_CUSTOM_COLOR = get_color('#EEEEEE')
#flow graph color constants
FLOWGRAPH_BACKGROUND_COLOR = get_color('#FFF9FF')
#block color constants
BLOCK_ENABLED_COLOR = get_color('#F1ECFF')
BLOCK_DISABLED_COLOR = get_color('#CCCCCC')
#connection color constants
CONNECTION_ENABLED_COLOR = get_color('black')
CONNECTION_DISABLED_COLOR = get_color('#999999')
CONNECTION_ERROR_COLOR = get_color('red')
