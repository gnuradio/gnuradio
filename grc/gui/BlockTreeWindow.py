"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
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

from Constants import DEFAULT_BLOCKS_WINDOW_WIDTH, DND_TARGETS
import Utils
import pygtk
pygtk.require('2.0')
import gtk
import gobject

NAME_INDEX = 0
KEY_INDEX = 1
DOC_INDEX = 2

DOC_MARKUP_TMPL="""\
#if $doc
$encode($doc)#slurp
#else
undocumented#slurp
#end if"""

CAT_MARKUP_TMPL="""Category: $cat"""

class BlockTreeWindow(gtk.VBox):
	"""The block selection panel."""

	def __init__(self, platform, get_flow_graph):
		"""
		BlockTreeWindow constructor.
		Create a tree view of the possible blocks in the platform.
		The tree view nodes will be category names, the leaves will be block names.
		A mouse double click or button press action will trigger the add block event.
		@param platform the particular platform will all block prototypes
		@param get_flow_graph get the selected flow graph
		"""
		gtk.VBox.__init__(self)
		self.platform = platform
		self.get_flow_graph = get_flow_graph
		#make the tree model for holding blocks
		self.treestore = gtk.TreeStore(gobject.TYPE_STRING, gobject.TYPE_STRING, gobject.TYPE_STRING)
		self.treeview = gtk.TreeView(self.treestore)
		self.treeview.set_enable_search(False) #disable pop up search box
		self.treeview.add_events(gtk.gdk.BUTTON_PRESS_MASK)
		self.treeview.connect('button-press-event', self._handle_mouse_button_press)
		selection = self.treeview.get_selection()
		selection.set_mode('single')
		selection.connect('changed', self._handle_selection_change)
		renderer = gtk.CellRendererText()
		column = gtk.TreeViewColumn('Blocks', renderer, text=NAME_INDEX)
		self.treeview.append_column(column)
		#setup the search
		self.treeview.set_enable_search(True)
		self.treeview.set_search_equal_func(self._handle_search)
		#try to enable the tooltips (available in pygtk 2.12 and above) 
		try: self.treeview.set_tooltip_column(DOC_INDEX)
		except: pass
		#setup drag and drop
		self.treeview.enable_model_drag_source(gtk.gdk.BUTTON1_MASK, DND_TARGETS, gtk.gdk.ACTION_COPY)
		self.treeview.connect('drag-data-get', self._handle_drag_get_data)
		#make the scrolled window to hold the tree view
		scrolled_window = gtk.ScrolledWindow()
		scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		scrolled_window.add_with_viewport(self.treeview)
		scrolled_window.set_size_request(DEFAULT_BLOCKS_WINDOW_WIDTH, -1)
		self.pack_start(scrolled_window)
		#add button
		self.add_button = gtk.Button(None, gtk.STOCK_ADD)
		self.add_button.connect('clicked', self._handle_add_button)
		self.pack_start(self.add_button, False)
		#map categories to iters, automatic mapping for root
		self._categories = {tuple(): None}
		#add blocks and categories
		self.platform.load_block_tree(self)
		#initialize
		self._update_add_button()

	############################################################
	## Block Tree Methods
	############################################################
	def add_block(self, category, block=None):
		"""
		Add a block with category to this selection window.
		Add only the category when block is None.
		@param category the category list or path string
		@param block the block object or None
		"""
		if isinstance(category, str): category = category.split('/')
		category = tuple(filter(lambda x: x, category)) #tuple is hashable
		#add category and all sub categories
		for i, cat_name in enumerate(category):
			sub_category = category[:i+1]
			if sub_category not in self._categories:
				iter = self.treestore.insert_before(self._categories[sub_category[:-1]], None)
				self.treestore.set_value(iter, NAME_INDEX, '[ %s ]'%cat_name)
				self.treestore.set_value(iter, KEY_INDEX, '')
				self.treestore.set_value(iter, DOC_INDEX, Utils.parse_template(CAT_MARKUP_TMPL, cat=cat_name))
				self._categories[sub_category] = iter
		#add block
		if block is None: return
		iter = self.treestore.insert_before(self._categories[category], None)
		self.treestore.set_value(iter, NAME_INDEX, block.get_name())
		self.treestore.set_value(iter, KEY_INDEX, block.get_key())
		self.treestore.set_value(iter, DOC_INDEX, Utils.parse_template(DOC_MARKUP_TMPL, doc=block.get_doc()))

	############################################################
	## Helper Methods
	############################################################
	def _get_selected_block_key(self):
		"""
		Get the currently selected block key.
		@return the key of the selected block or a empty string
		"""
		selection = self.treeview.get_selection()
		treestore, iter = selection.get_selected()
		return iter and treestore.get_value(iter, KEY_INDEX) or ''

	def _update_add_button(self):
		"""
		Update the add button's sensitivity.
		The button should be active only if a block is selected.
		"""
		key = self._get_selected_block_key()
		self.add_button.set_sensitive(bool(key))

	def _add_selected_block(self):
		"""
		Add the selected block with the given key to the flow graph.
		"""
		key = self._get_selected_block_key()
		if key: self.get_flow_graph().add_new_block(key)

	############################################################
	## Event Handlers
	############################################################
	def _handle_search(self, model, column, key, iter):
		#determine which blocks match the search key
		blocks = self.get_flow_graph().get_parent().get_blocks()
		matching_blocks = filter(lambda b: key in b.get_key() or key in b.get_name().lower(), blocks)
		#remove the old search category
		try: self.treestore.remove(self._categories.pop((self._search_category, )))
		except (KeyError, AttributeError): pass #nothing to remove
		#create a search category
		if not matching_blocks: return
		self._search_category = 'Search: %s'%key
		for block in matching_blocks: self.add_block(self._search_category, block)
		#expand the search category
		path = self.treestore.get_path(self._categories[(self._search_category, )])
		self.treeview.collapse_all()
		self.treeview.expand_row(path, open_all=False)

	def _handle_drag_get_data(self, widget, drag_context, selection_data, info, time):
		"""
		Handle a drag and drop by setting the key to the selection object.
		This will call the destination handler for drag and drop.
		Only call set when the key is valid to ignore DND from categories.
		"""
		key = self._get_selected_block_key()
		if key: selection_data.set(selection_data.target, 8, key)

	def _handle_mouse_button_press(self, widget, event):
		"""
		Handle the mouse button press.
		If a left double click is detected, call add selected block.
		"""
		if event.button == 1 and event.type == gtk.gdk._2BUTTON_PRESS:
			self._add_selected_block()

	def _handle_selection_change(self, selection):
		"""
		Handle a selection change in the tree view.
		If a selection changes, set the add button sensitive.
		"""
		self._update_add_button()

	def _handle_add_button(self, widget):
		"""
		Handle the add button clicked signal.
		Call add selected block.
		"""
		self._add_selected_block()
