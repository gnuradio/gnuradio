"""
Copyright 2007 Free Software Foundation, Inc.
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

from ... gui.Constants import \
	DIR_LEFT, DIR_RIGHT, \
	SCROLL_PROXIMITY_SENSITIVITY, SCROLL_DISTANCE, \
	MOTION_DETECT_REDRAWING_SENSITIVITY
from ... gui.Actions import \
	ELEMENT_CREATE, ELEMENT_SELECT, \
	BLOCK_PARAM_MODIFY, BLOCK_MOVE, \
	ELEMENT_DELETE
import Colors
import Utils
from ... import utils
from ... gui.ParamsDialog import ParamsDialog
from Element import Element
from .. base import FlowGraph as _FlowGraph
import pygtk
pygtk.require('2.0')
import gtk
import random
import time
from ... gui import Messages

class FlowGraph(Element):
	"""
	FlowGraph is the data structure to store graphical signal blocks,
	graphical inputs and outputs,
	and the connections between inputs and outputs.
	"""

	def __init__(self, *args, **kwargs):
		"""
		FlowGraph contructor.
		Create a list for signal blocks and connections. Connect mouse handlers.
		"""
		Element.__init__(self)
		#when is the flow graph selected? (used by keyboard event handler)
		self.is_selected = lambda: bool(self.get_selected_elements())
		#important vars dealing with mouse event tracking
		self.element_moved = False
		self.mouse_pressed = False
		self.unselect()
		self.time = 0
		self.press_coor = (0, 0)
		#selected ports
		self._old_selected_port = None
		self._new_selected_port = None

	###########################################################################
	# Access Drawing Area
	###########################################################################
	def get_drawing_area(self): return self.drawing_area
	def get_gc(self): return self.get_drawing_area().gc
	def get_pixmap(self): return self.get_drawing_area().pixmap
	def get_size(self): return self.get_drawing_area().get_size_request()
	def set_size(self, *args): self.get_drawing_area().set_size_request(*args)
	def get_window(self): return self.get_drawing_area().window
	def get_scroll_pane(self): return self.drawing_area.get_parent()
	def get_ctrl_mask(self): return self.drawing_area.ctrl_mask

	def add_new_block(self, key):
		"""
		Add a block of the given key to this flow graph.
		@param key the block key
		"""
		id = self._get_unique_id(key)
		#calculate the position coordinate
		h_adj = self.get_scroll_pane().get_hadjustment()
		v_adj = self.get_scroll_pane().get_vadjustment()
		x = int(random.uniform(.25, .75)*h_adj.page_size + h_adj.get_value())
		y = int(random.uniform(.25, .75)*v_adj.page_size + v_adj.get_value())
		#get the new block
		block = self.get_new_block(key)
		block.set_coordinate((x, y))
		block.set_rotation(0)
		block.get_param('id').set_value(id)
		self.handle_states(ELEMENT_CREATE)

	###########################################################################
	# Copy Paste
	###########################################################################
	def copy_to_clipboard(self):
		"""
		Copy the selected blocks and connections into the clipboard.
		@return the clipboard
		"""
		#get selected blocks
		blocks = self.get_selected_blocks()
		if not blocks: return None
		#calc x and y min
		x_min, y_min = blocks[0].get_coordinate()
		for block in blocks:
			x, y = block.get_coordinate()
			x_min = min(x, x_min)
			y_min = min(y, y_min)
		#get connections between selected blocks
		connections = filter(
			lambda c: c.get_source().get_parent() in blocks and c.get_sink().get_parent() in blocks,
			self.get_connections(),
		)
		clipboard = (
			(x_min, y_min),
			[block.export_data() for block in blocks],
			[connection.export_data() for connection in connections],
		)
		return clipboard

	def paste_from_clipboard(self, clipboard):
		"""
		Paste the blocks and connections from the clipboard.
		@param clipboard the nested data of blocks, connections
		"""
		selected = set()
		(x_min, y_min), blocks_n, connections_n = clipboard
		old_id2block = dict()
		#recalc the position
		h_adj = self.get_scroll_pane().get_hadjustment()
		v_adj = self.get_scroll_pane().get_vadjustment()
		x_off = h_adj.get_value() - x_min + h_adj.page_size/4
		y_off = v_adj.get_value() - y_min + v_adj.page_size/4
		#create blocks
		for block_n in blocks_n:
			block_key = block_n['key']
			if block_key == 'options': continue
			block = self.get_new_block(block_key)
			selected.add(block)
			#set params
			params_n = utils.listify(block_n, 'param')
			for param_n in params_n:
				param_key = param_n['key']
				param_value = param_n['value']
				#setup id parameter
				if param_key == 'id':
					old_id2block[param_value] = block
					#if the block id is not unique, get a new block id
					if param_value in [block.get_id() for block in self.get_blocks()]:
						param_value = self._get_unique_id(param_value)
				#set value to key
				block.get_param(param_key).set_value(param_value)
			#move block to offset coordinate
			block.move((x_off, y_off))
		#update before creating connections
		self.update()
		#create connections
		for connection_n in connections_n:
			source = old_id2block[connection_n['source_block_id']].get_source(connection_n['source_key'])
			sink = old_id2block[connection_n['sink_block_id']].get_sink(connection_n['sink_key'])
			self.connect(source, sink)
		#set all pasted elements selected
		for block in selected: selected = selected.union(set(block.get_connections()))
		self._selected_elements = list(selected)

	###########################################################################
	# Modify Selected
	###########################################################################
	def type_controller_modify_selected(self, direction):
		"""
		Change the registered type controller for the selected signal blocks.
		@param direction +1 or -1
		@return true for change
		"""
		return any([sb.type_controller_modify(direction) for sb in self.get_selected_blocks()])

	def port_controller_modify_selected(self, direction):
		"""
		Change port controller for the selected signal blocks.
		@param direction +1 or -1
		@return true for changed
		"""
		return any([sb.port_controller_modify(direction) for sb in self.get_selected_blocks()])

	def param_modify_selected(self):
		"""
		Create and show a param modification dialog for the selected block.
		@return true if parameters were changed
		"""
		if not self.get_selected_block(): return False
		return ParamsDialog(self.get_selected_block()).run()

	def enable_selected(self, enable):
		"""
		Enable/disable the selected blocks.
		@param enable true to enable
		@return true if changed
		"""
		changed = False
		for selected_block in self.get_selected_blocks():
			if selected_block.get_enabled() != enable:
				selected_block.set_enabled(enable)
				changed = True
		return changed

	def move_selected(self, delta_coordinate):
		"""
		Move the element and by the change in coordinates.
		@param delta_coordinate the change in coordinates
		"""
		for selected_block in self.get_selected_blocks():
			selected_block.move(delta_coordinate)
			self.element_moved = True

	def rotate_selected(self, direction):
		"""
		Rotate the selected blocks by 90 degrees.
		@param direction DIR_LEFT or DIR_RIGHT
		@return true if changed, otherwise false.
		"""
		if not self.get_selected_blocks(): return False
		#determine the number of degrees to rotate
		rotation = {DIR_LEFT: 90, DIR_RIGHT:270}[direction]
		#initialize min and max coordinates
		min_x, min_y = self.get_selected_block().get_coordinate()
		max_x, max_y = self.get_selected_block().get_coordinate()
		#rotate each selected block, and find min/max coordinate
		for selected_block in self.get_selected_blocks():
			selected_block.rotate(rotation)
			#update the min/max coordinate
			x, y = selected_block.get_coordinate()
			min_x, min_y = min(min_x, x), min(min_y, y)
			max_x, max_y = max(max_x, x), max(max_y, y)
		#calculate center point of slected blocks
		ctr_x, ctr_y = (max_x + min_x)/2, (max_y + min_y)/2
		#rotate the blocks around the center point
		for selected_block in self.get_selected_blocks():
			x, y = selected_block.get_coordinate()
			x, y = Utils.get_rotated_coordinate((x - ctr_x, y - ctr_y), rotation)
			selected_block.set_coordinate((x + ctr_x, y + ctr_y))
		return True

	def remove_selected(self):
		"""
		Remove selected elements
		@return true if changed.
		"""
		changed = False
		for selected_element in self.get_selected_elements():
			self.remove_element(selected_element)
			changed = True
		return changed

	def draw(self):
		"""
		Draw the background and grid if enabled.
		Draw all of the elements in this flow graph onto the pixmap.
		Draw the pixmap to the drawable window of this flow graph.
		"""
		if self.get_gc():
			W,H = self.get_size()
			#draw the background
			self.get_gc().foreground = Colors.BACKGROUND_COLOR
			self.get_pixmap().draw_rectangle(self.get_gc(), True, 0, 0, W, H)
			#draw multi select rectangle
			if self.mouse_pressed and (not self.get_selected_elements() or self.get_ctrl_mask()):
				#coordinates
				x1, y1 = self.press_coor
				x2, y2 = self.get_coordinate()
				#calculate top-left coordinate and width/height
				x, y = int(min(x1, x2)), int(min(y1, y2))
				w, h = int(abs(x1 - x2)), int(abs(y1 - y2))
				#draw
				self.get_gc().foreground = Colors.H_COLOR
				self.get_pixmap().draw_rectangle(self.get_gc(), True, x, y, w, h)
				self.get_gc().foreground = Colors.TXT_COLOR
				self.get_pixmap().draw_rectangle(self.get_gc(), False, x, y, w, h)
			#draw blocks on top of connections
			for element in self.get_connections() + self.get_blocks():
				element.draw(self.get_pixmap())
			#draw selected blocks on top of selected connections
			for selected_element in self.get_selected_connections() + self.get_selected_blocks():
				selected_element.draw(self.get_pixmap())
			self.get_drawing_area().draw()

	def update(self):
		"""
		Update highlighting so only the selected is highlighted.
		Call update on all elements.
		Resize the window if size changed.
		"""
		#update highlighting
		map(lambda e: e.set_highlighted(False), self.get_elements())
		for selected_element in self.get_selected_elements():
			selected_element.set_highlighted(True)
		#update all elements
		map(lambda e: e.update(), self.get_elements())
		#set the size of the flow graph area
		old_x, old_y = self.get_size()
		try: new_x, new_y = self.get_option('window_size')
		except: new_x, new_y = old_x, old_y
		if new_x != old_x or new_y != old_y: self.set_size(new_x, new_y)

	##########################################################################
	## Get Selected
	##########################################################################
	def unselect(self):
		"""
		Set selected elements to an empty set.
		"""
		self._selected_elements = []

	def what_is_selected(self, coor, coor_m=None):
		"""
		What is selected?
		At the given coordinate, return the elements found to be selected.
		If coor_m is unspecified, return a list of only the first element found to be selected:
		Iterate though the elements backwardssince top elements are at the end of the list.
		If an element is selected, place it at the end of the list so that is is drawn last,
		and hence on top. Update the selected port information.
		@param coor the coordinate of the mouse click
		@param coor_m the coordinate for multi select
		@return the selected blocks and connections or an empty list
		"""
		selected_port = None
		selected = set()
		#check the elements
		for element in reversed(self.get_elements()):
			selected_element = element.what_is_selected(coor, coor_m)
			if not selected_element: continue
			#update the selected port information
			if selected_element.is_port():
				if not coor_m: selected_port = selected_element
				selected_element = selected_element.get_parent()
			selected.add(selected_element)
			#single select mode, break
			if not coor_m:
				self.get_elements().remove(element)
				self.get_elements().append(element)
				break;
		#update selected ports
		self._old_selected_port = self._new_selected_port
		self._new_selected_port = selected_port
		return list(selected)

	def get_selected_connections(self):
		"""
		Get a group of selected connections.
		@return sub set of connections in this flow graph
		"""
		selected = set()
		for selected_element in self.get_selected_elements():
			if selected_element.is_connection(): selected.add(selected_element)
		return list(selected)

	def get_selected_blocks(self):
		"""
		Get a group of selected blocks.
		@return sub set of blocks in this flow graph
		"""
		selected = set()
		for selected_element in self.get_selected_elements():
			if selected_element.is_block(): selected.add(selected_element)
		return list(selected)

	def get_selected_block(self):
		"""
		Get the selected block when a block or port is selected.
		@return a block or None
		"""
		return self.get_selected_blocks() and self.get_selected_blocks()[0] or None

	def get_selected_elements(self):
		"""
		Get the group of selected elements.
		@return sub set of elements in this flow graph
		"""
		return self._selected_elements

	def get_selected_element(self):
		"""
		Get the selected element.
		@return a block, port, or connection or None
		"""
		return self.get_selected_elements() and self.get_selected_elements()[0] or None

	def update_selected_elements(self):
		"""
		Update the selected elements.
		The update behavior depends on the state of the mouse button.
		When the mouse button pressed the selection will change when
		the control mask is set or the new selection is not in the current group.
		When the mouse button is released the selection will change when
		the mouse has moved and the control mask is set or the current group is empty.
		Attempt to make a new connection if the old and ports are filled.
		If the control mask is set, merge with the current elements.
		"""
		selected_elements = None
		if self.mouse_pressed:
			new_selections = self.what_is_selected(self.get_coordinate())
			#update the selections if the new selection is not in the current selections
			#allows us to move entire selected groups of elements
			if self.get_ctrl_mask() or not (
				new_selections and new_selections[0] in self.get_selected_elements()
			): selected_elements = new_selections
		else: #called from a mouse release
			if not self.element_moved and (not self.get_selected_elements() or self.get_ctrl_mask()):
				selected_elements = self.what_is_selected(self.get_coordinate(), self.press_coor)
		#this selection and the last were ports, try to connect them
		if self._old_selected_port and self._new_selected_port and \
			self._old_selected_port is not self._new_selected_port:
			try:
				self.connect(self._old_selected_port, self._new_selected_port)
				self.handle_states(ELEMENT_CREATE)
			except: Messages.send_fail_connection()
			self._old_selected_port = None
			self._new_selected_port = None
			return
		#update selected elements
		if selected_elements is None: return
		old_elements = set(self.get_selected_elements())
		self._selected_elements = list(set(selected_elements))
		new_elements = set(self.get_selected_elements())
		#if ctrl, set the selected elements to the union - intersection of old and new
		if self.get_ctrl_mask():
			self._selected_elements = list(
				set.union(old_elements, new_elements) - set.intersection(old_elements, new_elements)
			)
		self.handle_states(ELEMENT_SELECT)

	##########################################################################
	## Event Handlers
	##########################################################################
	def handle_mouse_button_press(self, left_click, double_click, coordinate):
		"""
		A mouse button is pressed, only respond to left clicks.
		Find the selected element. Attempt a new connection if possible.
		Open the block params window on a double click.
		Update the selection state of the flow graph.
		"""
		if not left_click: return
		self.press_coor = coordinate
		self.set_coordinate(coordinate)
		self.time = 0
		self.mouse_pressed = True
		if double_click: self.unselect()
		self.update_selected_elements()
		#double click detected, bring up params dialog if possible
		if double_click and self.get_selected_block():
			self.mouse_pressed = False
			self.handle_states(BLOCK_PARAM_MODIFY)

	def handle_mouse_button_release(self, left_click, coordinate):
		"""
		A mouse button is released, record the state.
		"""
		if not left_click: return
		self.set_coordinate(coordinate)
		self.time = 0
		self.mouse_pressed = False
		if self.element_moved:
			self.handle_states(BLOCK_MOVE)
			self.element_moved = False
		self.update_selected_elements()
		self.draw()

	def handle_mouse_motion(self, coordinate):
		"""
		The mouse has moved, respond to mouse dragging.
		Move a selected element to the new coordinate.
		Auto-scroll the scroll bars at the boundaries.
		"""
		#to perform a movement, the mouse must be pressed, timediff large enough
		if not self.mouse_pressed: return
		if time.time() - self.time < MOTION_DETECT_REDRAWING_SENSITIVITY: return
		#perform autoscrolling
		width, height = self.get_size()
		x, y = coordinate
		h_adj = self.get_scroll_pane().get_hadjustment()
		v_adj = self.get_scroll_pane().get_vadjustment()
		for pos, length, adj, adj_val, adj_len in (
			(x, width, h_adj, h_adj.get_value(), h_adj.page_size),
			(y, height, v_adj, v_adj.get_value(), v_adj.page_size),
		):
			#scroll if we moved near the border
			if pos-adj_val > adj_len-SCROLL_PROXIMITY_SENSITIVITY and adj_val+SCROLL_DISTANCE < length-adj_len:
				adj.set_value(adj_val+SCROLL_DISTANCE)
				adj.emit('changed')
			elif pos-adj_val < SCROLL_PROXIMITY_SENSITIVITY:
				adj.set_value(adj_val-SCROLL_DISTANCE)
				adj.emit('changed')
		#remove the connection if selected in drag event
		if len(self.get_selected_elements()) == 1 and self.get_selected_element().is_connection():
			self.handle_states(ELEMENT_DELETE)
		#move the selected elements and record the new coordinate
		X, Y = self.get_coordinate()
		if not self.get_ctrl_mask(): self.move_selected((int(x - X), int(y - Y)))
		self.draw()
		self.set_coordinate((x, y))
		#update time
		self.time = time.time()
