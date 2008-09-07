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

COLORMAP = gtk.gdk.colormap_get_system() #create all of the colors
def get_color(color_code): return COLORMAP.alloc_color(color_code, True, True)

BACKGROUND_COLOR = get_color('#FFF9FF') #main window background
FG_COLOR = get_color('black') #normal border color
BG_COLOR = get_color('#F1ECFF') #default background
DISABLED_BG_COLOR = get_color('#CCCCCC') #disabled background
DISABLED_FG_COLOR = get_color('#999999') #disabled foreground
H_COLOR = get_color('#00FFFF') #Highlight border color
TXT_COLOR = get_color('black') #text color
ERROR_COLOR = get_color('red') #error color
