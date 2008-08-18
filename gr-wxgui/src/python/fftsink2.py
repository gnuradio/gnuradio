#
# Copyright 2008 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr

p = gr.prefs()
style = p.get_string('wxgui', 'style', 'auto')

# In 3.2 we'll change 'auto' to mean 'gl' if possible, then fallback
# Currently, anything other than 'gl' means 'nongl'

if style == 'gl':
    try:
        import wx.glcanvas
    except ImportError:
        raise RuntimeError("wxPython doesn't support glcanvas")

    try:
        from OpenGL.GL import *
    except ImportError:
        raise RuntimeError("Unable to import OpenGL. Are Python wrappers for OpenGL installed?")

    from fftsink_gl import fft_sink_f, fft_sink_c

else:
    from fftsink_nongl import fft_sink_f, fft_sink_c
