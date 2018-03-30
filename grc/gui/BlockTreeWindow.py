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

import pygtk
pygtk.require('2.0')
import gtk
import gobject

from . import Actions, Utils
from . import Constants


NAME_INDEX = 0
KEY_INDEX = 1
DOC_INDEX = 2

DOC_MARKUP_TMPL = """\
#set $docs = []
#if $doc.get('')
    #set $docs += $doc.pop('').splitlines() + ['']
#end if
#for b, d in $doc.iteritems()
    #set $docs += ['--- {0} ---'.format(b)] + d.splitlines() + ['']
#end for
#set $len_out = 0
#for $n, $line in $enumerate($docs[:-1])
#if $n

#end if
$encode($line)#slurp
#set $len_out += $len($line)
#if $n > 10 or $len_out > 500

...#slurp
#break
#end if
#end for
#if $len_out == 0
undocumented#slurp
#end if"""

CAT_MARKUP_TMPL = """
#set $name = $cat[-1]
#if len($cat) > 1
#silent from xml.sax.saxutils import escape
Category: #echo escape($cat[-1])
##
#elif $name == 'Core'
Module: Core

This subtree is meant for blocks included with GNU Radio (in-tree).
##
#elif $name == $default_module
This subtree holds all blocks (from OOT modules) that specify no module name. \
The module name is the root category enclosed in square brackets.

Please consider contacting OOT module maintainer for any block in here \
and kindly ask to update their GRC Block Descriptions or Block Tree to include a module name.
#else
Module: $name
##
#end if
""".strip()


class BlockTreeWindow(gtk.VBox):
    """The block selection panel."""

    def __init__(self, platform, get_flow_graph):
        """
        BlockTreeWindow constructor.
        Create a tree view of the possible blocks in the platform.
        The tree view nodes will be category names, the leaves will be block names.
        A mouse double click or button press action will trigger the add block event.

        Args:
            platform: the particular platform will all block prototypes
            get_flow_graph: get the selected flow graph
        """
        gtk.VBox.__init__(self)
        self.platform = platform
        self.get_flow_graph = get_flow_graph

        # search entry
        self.search_entry = gtk.Entry()
        try:
            self.search_entry.set_icon_from_stock(gtk.ENTRY_ICON_PRIMARY, gtk.STOCK_FIND)
            self.search_entry.set_icon_activatable(gtk.ENTRY_ICON_PRIMARY, False)
            self.search_entry.set_icon_from_stock(gtk.ENTRY_ICON_SECONDARY, gtk.STOCK_CLOSE)
            self.search_entry.connect('icon-release', self._handle_icon_event)
        except AttributeError:
            pass  # no icon for old pygtk
        self.search_entry.connect('changed', self._update_search_tree)
        self.search_entry.connect('key-press-event', self._handle_search_key_press)
        self.pack_start(self.search_entry, False)

        # make the tree model for holding blocks and a temporary one for search results
        self.treestore = gtk.TreeStore(gobject.TYPE_STRING, gobject.TYPE_STRING, gobject.TYPE_STRING)
        self.treestore_search = gtk.TreeStore(gobject.TYPE_STRING, gobject.TYPE_STRING, gobject.TYPE_STRING)

        self.treeview = gtk.TreeView(self.treestore)
        self.treeview.set_enable_search(False)  # disable pop up search box
        self.treeview.set_search_column(-1)  # really disable search
        self.treeview.set_headers_visible(False)
        self.treeview.add_events(gtk.gdk.BUTTON_PRESS_MASK)
        self.treeview.connect('button-press-event', self._handle_mouse_button_press)
        self.treeview.connect('key-press-event', self._handle_search_key_press)

        self.treeview.get_selection().set_mode('single')
        renderer = gtk.CellRendererText()
        column = gtk.TreeViewColumn('Blocks', renderer, text=NAME_INDEX)
        self.treeview.append_column(column)
        # try to enable the tooltips (available in pygtk 2.12 and above)
        try:
            self.treeview.set_tooltip_column(DOC_INDEX)
        except:
            pass
        # setup sort order
        column.set_sort_column_id(0)
        self.treestore.set_sort_column_id(0, gtk.SORT_ASCENDING)
        # setup drag and drop
        self.treeview.enable_model_drag_source(gtk.gdk.BUTTON1_MASK, Constants.DND_TARGETS, gtk.gdk.ACTION_COPY)
        self.treeview.connect('drag-data-get', self._handle_drag_get_data)
        # make the scrolled window to hold the tree view
        scrolled_window = gtk.ScrolledWindow()
        scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scrolled_window.add_with_viewport(self.treeview)
        scrolled_window.set_size_request(Constants.DEFAULT_BLOCKS_WINDOW_WIDTH, -1)
        self.pack_start(scrolled_window)
        # map categories to iters, automatic mapping for root
        self._categories = {tuple(): None}
        self._categories_search = {tuple(): None}
        self.platform.block_docstrings_loaded_callback = self.update_docs
        self.repopulate()

    def clear(self):
        self.treestore.clear()
        self._categories = {(): None}

    def repopulate(self):
        self.clear()
        for block in self.platform.blocks.itervalues():
            if block.category:
                self.add_block(block)
        self.expand_module_in_tree()

    def expand_module_in_tree(self, module_name='Core'):
        self.treeview.collapse_all()
        core_module_iter = self._categories.get((module_name,))
        if core_module_iter:
            self.treeview.expand_row(self.treestore.get_path(core_module_iter), False)

    ############################################################
    ## Block Tree Methods
    ############################################################
    def add_block(self, block, treestore=None, categories=None):
        """
        Add a block with category to this selection window.
        Add only the category when block is None.

        Args:
            block: the block object or None
        """
        treestore = treestore or self.treestore
        categories = categories or self._categories

        category = tuple(filter(str, block.category))  # tuple is hashable, remove empty cats

        # add category and all sub categories
        for level, parent_cat_name in enumerate(category, 1):
            parent_category = category[:level]
            if parent_category not in categories:
                iter_ = treestore.insert_before(categories[parent_category[:-1]], None)
                treestore.set_value(iter_, NAME_INDEX, parent_cat_name)
                treestore.set_value(iter_, KEY_INDEX, '')
                treestore.set_value(iter_, DOC_INDEX, Utils.parse_template(
                    CAT_MARKUP_TMPL, cat=parent_category, default_module=Constants.DEFAULT_BLOCK_MODULE_NAME))
                categories[parent_category] = iter_

        # add block
        iter_ = treestore.insert_before(categories[category], None)
        treestore.set_value(iter_, NAME_INDEX, block.get_name())
        treestore.set_value(iter_, KEY_INDEX, block.get_key())
        treestore.set_value(iter_, DOC_INDEX, Utils.parse_template(DOC_MARKUP_TMPL, doc=block.get_doc()))

    def update_docs(self):
        """Update the documentation column of every block"""

        def update_doc(model, _, iter_):
            key = model.get_value(iter_, KEY_INDEX)
            if not key:
                return  # category node, no doc string
            block = self.platform.blocks[key]
            doc = Utils.parse_template(DOC_MARKUP_TMPL, doc=block.get_doc())
            model.set_value(iter_, DOC_INDEX, doc)

        self.treestore.foreach(update_doc)
        self.treestore_search.foreach(update_doc)

    ############################################################
    ## Helper Methods
    ############################################################
    def _get_selected_block_key(self):
        """
        Get the currently selected block key.

        Returns:
            the key of the selected block or a empty string
        """
        selection = self.treeview.get_selection()
        treestore, iter = selection.get_selected()
        return iter and treestore.get_value(iter, KEY_INDEX) or ''

    def _add_selected_block(self):
        """
        Add the selected block with the given key to the flow graph.
        """
        key = self._get_selected_block_key()
        if key:
            self.get_flow_graph().add_new_block(key)
            return True
        return False

    def _expand_category(self):
        treestore, iter = self.treeview.get_selection().get_selected()
        if iter and treestore.iter_has_child(iter):
            path = treestore.get_path(iter)
            self.treeview.expand_to_path(path)

    ############################################################
    ## Event Handlers
    ############################################################
    def _handle_icon_event(self, widget, icon, event):
        if icon == gtk.ENTRY_ICON_PRIMARY:
            pass
        elif icon == gtk.ENTRY_ICON_SECONDARY:
            widget.set_text('')
            self.search_entry.hide()

    def _update_search_tree(self, widget):
        key = widget.get_text().lower()
        if not key:
            self.treeview.set_model(self.treestore)
            self.expand_module_in_tree()
        else:
            matching_blocks = filter(lambda b: key in b.get_key().lower() or key in b.get_name().lower(),
                                     self.platform.blocks.values())

            self.treestore_search.clear()
            self._categories_search = {tuple(): None}
            for block in matching_blocks:
                self.add_block(block, self.treestore_search, self._categories_search)
            self.treeview.set_model(self.treestore_search)
            self.treeview.expand_all()

    def _handle_search_key_press(self, widget, event):
        """Handle Return and Escape key events in search entry and treeview"""
        if event.keyval == gtk.keysyms.Return:
            # add block on enter

            if widget == self.search_entry:
                #  Get the first block in the search tree and add it
                selected = self.treestore_search.get_iter_first()
                while self.treestore_search.iter_children(selected):
                    selected = self.treestore_search.iter_children(selected)
                if selected is not None:
                    key = self.treestore_search.get_value(selected, KEY_INDEX)
                    if key: self.get_flow_graph().add_new_block(key)
            elif widget == self.treeview:
                self._add_selected_block() or self._expand_category()
            else:
                return False  # propagate event

        elif event.keyval == gtk.keysyms.Escape:
            # reset the search
            self.search_entry.set_text('')
            self.search_entry.hide()

        elif (event.state & gtk.gdk.CONTROL_MASK and event.keyval == gtk.keysyms.f) \
             or event.keyval == gtk.keysyms.slash:
            # propagation doesn't work although treeview search is disabled =(
            # manually trigger action...
            Actions.FIND_BLOCKS.activate()

        elif event.state & gtk.gdk.CONTROL_MASK and event.keyval == gtk.keysyms.b:
            # ugly...
            Actions.TOGGLE_BLOCKS_WINDOW.activate()

        else:
            return False # propagate event

        return True

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
