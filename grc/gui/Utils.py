"""
Copyright 2008-2011 Free Software Foundation, Inc.
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

from Constants import POSSIBLE_ROTATIONS
from Cheetah.Template import Template
import pygtk
pygtk.require('2.0')
import gtk
import gobject

def rotate_pixmap(gc, src_pixmap, dst_pixmap, angle=gtk.gdk.PIXBUF_ROTATE_COUNTERCLOCKWISE):
	"""
	Load the destination pixmap with a rotated version of the source pixmap.
	The source pixmap will be loaded into a pixbuf, rotated, and drawn to the destination pixmap.
	The pixbuf is a client-side drawable, where a pixmap is a server-side drawable.
	@param gc the graphics context
	@param src_pixmap the source pixmap
	@param dst_pixmap the destination pixmap
	@param angle the angle to rotate by
	"""
	width, height = src_pixmap.get_size()
	pixbuf = gtk.gdk.Pixbuf(
		colorspace=gtk.gdk.COLORSPACE_RGB,
		has_alpha=False, bits_per_sample=8,
		width=width, height=height,
	)
	pixbuf.get_from_drawable(src_pixmap, src_pixmap.get_colormap(), 0, 0, 0, 0, -1, -1)
	pixbuf = pixbuf.rotate_simple(angle)
	dst_pixmap.draw_pixbuf(gc, pixbuf, 0, 0, 0, 0)

def get_rotated_coordinate(coor, rotation):
	"""
	Rotate the coordinate by the given rotation.
	@param coor the coordinate x, y tuple
	@param rotation the angle in degrees
	@return the rotated coordinates
	"""
	#handles negative angles
	rotation = (rotation + 360)%360
	if rotation not in POSSIBLE_ROTATIONS:
		raise ValueError('unusable rotation angle "%s"'%str(rotation))
	#determine the number of degrees to rotate
	cos_r, sin_r = {
		0: (1, 0),
		90: (0, 1),
		180: (-1, 0),
		270: (0, -1),
	}[rotation]
	x, y = coor
	return (x*cos_r + y*sin_r, -x*sin_r + y*cos_r)

def get_angle_from_coordinates((x1,y1), (x2,y2)):
	"""
	Given two points, calculate the vector direction from point1 to point2, directions are multiples of 90 degrees.
	@param (x1,y1) the coordinate of point 1
	@param (x2,y2) the coordinate of point 2
	@return the direction in degrees
	"""
	if y1 == y2:#0 or 180
		if x2 > x1: return 0
		else: return 180
	else:#90 or 270
		if y2 > y1: return 270
		else: return 90

def parse_template(tmpl_str, **kwargs):
	"""
	Parse the template string with the given args.
	Pass in the xml encode method for pango escape chars.
	@param tmpl_str the template as a string
	@return a string of the parsed template
	"""
	kwargs['encode'] = gobject.markup_escape_text
	return str(Template(tmpl_str, kwargs))
