#
# Copyright 2008,2009 Free Software Foundation, Inc.
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

if style == 'auto' or style == 'gl':
    try:
        import wx.glcanvas
        from OpenGL.GL import *
        from waterfallsink_gl import waterfall_sink_f, waterfall_sink_c
    except ImportError:
        if style == 'gl':
            raise RuntimeError("Unable to import OpenGL.  Are Python wrappers for OpenGL installed?")
        else:
            # Fall backto non-gl sinks
            from waterfallsink_nongl import waterfall_sink_f, waterfall_sink_c
elif style == 'nongl':
    from waterfallsink_nongl import waterfall_sink_f, waterfall_sink_c
else:
    raise RuntimeError("Unknown wxgui style")
