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
##@package grc.gui.elements.Platform
#Graphical platform to turn an existing platform into a gui platform.

from FlowGraph import FlowGraph
from Connection import Connection
from Block import Block
from Port import Port
from Param import Param

def conjoin_classes(name, c1, c2):
	exec("""
class %s(c1, c2):
	def __init__(self, *args, **kwargs):
		c1.__init__(self, *args, **kwargs)
		c2.__init__(self, *args, **kwargs)
"""%name, locals())
	return locals()[name]	

def Platform(platform):
	#combine with gui class
	for attr, value in (
		('FlowGraph', FlowGraph),
		('Connection', Connection),
		('Block', Block),
		('Source', Port),
		('Sink', Port),
		('Param', Param),
	):
		old_value = getattr(platform, attr) 
		c = conjoin_classes(attr, old_value, value)		
		setattr(platform, attr, c)
	return platform
	
