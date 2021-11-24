"""
Copyright 2007, 2008, 2009, 2016 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

from gi.repository import Gtk, Gdk, GObject

from . import Actions, Utils, Constants


NAME_INDEX, KEY_INDEX, DOC_INDEX = range(3)


def _format_doc(doc):
    docs = []
    if doc.get(''):
        docs += doc.get('').splitlines() + ['']
    for block_name, docstring in doc.items():
        docs.append('--- {0} ---'.format(block_name))
        docs += docstring.splitlines()
        docs.append('')
    out = ''
    for n, line in enumerate(docs[:-1]):
        if n:
            out += '\n'
        out += Utils.encode(line)
        if n > 10 or len(out) > 500:
            out += '\n...'
            break
    return out or 'undocumented'


def _format_cat_tooltip(category):
    tooltip = '{}: {}'.format('Category' if len(
        category) > 1 else 'Module', category[-1])

    if category == ('Core',):
        tooltip += '\n\nThis subtree is meant for blocks included with GNU Radio (in-tree).'

    elif category == (Constants.DEFAULT_BLOCK_MODULE_NAME,):
        tooltip += '\n\n' + Constants.DEFAULT_BLOCK_MODULE_TOOLTIP

    return tooltip


class BlockTreeWindow(Gtk.VBox):
    """The block selection panel."""

    __gsignals__ = {
        'create_new_block': (GObject.SignalFlags.RUN_FIRST, None, (str,))
    }

    def __init__(self, platform):
        """
        BlockTreeWindow constructor.
        Create a tree view of the possible blocks in the platform.
        The tree view nodes will be category names, the leaves will be block names.
        A mouse double click or button press action will trigger the add block event.

        Args:
            platform: the particular platform will all block prototypes
        """
        Gtk.VBox.__init__(self)
        self.platform = platform

        # search entry
        self.search_entry = Gtk.Entry()
        try:
            self.search_entry.set_icon_from_icon_name(
                Gtk.EntryIconPosition.PRIMARY, 'edit-find')
            self.search_entry.set_icon_activatable(
                Gtk.EntryIconPosition.PRIMARY, False)
            self.search_entry.set_icon_from_icon_name(
                Gtk.EntryIconPosition.SECONDARY, 'window-close')
            self.search_entry.connect('icon-release', self._handle_icon_event)
        except AttributeError:
            pass  # no icon for old pygtk
        self.search_entry.connect('changed', self._update_search_tree)
        self.search_entry.connect(
            'key-press-event', self._handle_search_key_press)
        self.pack_start(self.search_entry, False, False, 0)

        # make the tree model for holding blocks and a temporary one for search results
        self.treestore = Gtk.TreeStore(
            GObject.TYPE_STRING, GObject.TYPE_STRING, GObject.TYPE_STRING)
        self.treestore_search = Gtk.TreeStore(
            GObject.TYPE_STRING, GObject.TYPE_STRING, GObject.TYPE_STRING)

        self.treeview = Gtk.TreeView(model=self.treestore)
        self.treeview.set_enable_search(False)  # disable pop up search box
        self.treeview.set_search_column(-1)  # really disable search
        self.treeview.set_headers_visible(False)
        self.treeview.add_events(Gdk.EventMask.BUTTON_PRESS_MASK)
        self.treeview.connect('button-press-event',
                              self._handle_mouse_button_press)
        self.treeview.connect('key-press-event', self._handle_search_key_press)

        self.treeview.get_selection().set_mode(Gtk.SelectionMode.SINGLE)
        renderer = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn('Blocks', renderer, text=NAME_INDEX)
        self.treeview.append_column(column)
        self.treeview.set_tooltip_column(DOC_INDEX)
        # setup sort order
        column.set_sort_column_id(0)
        self.treestore.set_sort_column_id(0, Gtk.SortType.ASCENDING)
        # setup drag and drop
        self.treeview.enable_model_drag_source(
            Gdk.ModifierType.BUTTON1_MASK, Constants.DND_TARGETS, Gdk.DragAction.COPY)
        self.treeview.connect('drag-data-get', self._handle_drag_get_data)
        # make the scrolled window to hold the tree view
        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(
            Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.add(self.treeview)
        scrolled_window.set_size_request(
            Constants.DEFAULT_BLOCKS_WINDOW_WIDTH, -1)
        self.pack_start(scrolled_window, True, True, 0)
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
        for block in self.platform.blocks.values():
            if block.category:
                self.add_block(block)
        self.expand_module_in_tree()

    def expand_module_in_tree(self, module_name='Core'):
        self.treeview.collapse_all()
        core_module_iter = self._categories.get((module_name,))
        if core_module_iter:
            self.treeview.expand_row(
                self.treestore.get_path(core_module_iter), False)

    ############################################################
    # Block Tree Methods
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

        # tuple is hashable, remove empty cats
        category = tuple(filter(str, block.category))

        # add category and all sub categories
        for level, parent_cat_name in enumerate(category, 1):
            parent_category = category[:level]
            if parent_category not in categories:
                iter_ = treestore.insert_before(
                    categories[parent_category[:-1]], None)
                treestore.set_value(iter_, NAME_INDEX, parent_cat_name)
                treestore.set_value(iter_, KEY_INDEX, '')
                treestore.set_value(
                    iter_, DOC_INDEX, _format_cat_tooltip(parent_category))
                categories[parent_category] = iter_
        # add block
        iter_ = treestore.insert_before(categories[category], None)
        treestore.set_value(iter_, KEY_INDEX, block.key)
        treestore.set_value(iter_, NAME_INDEX, block.label)
        treestore.set_value(iter_, DOC_INDEX, _format_doc(block.documentation))

    def update_docs(self):
        """Update the documentation column of every block"""

        def update_doc(model, _, iter_):
            key = model.get_value(iter_, KEY_INDEX)
            if not key:
                return  # category node, no doc string
            block = self.platform.blocks[key]
            model.set_value(iter_, DOC_INDEX, _format_doc(block.documentation))

        self.treestore.foreach(update_doc)
        self.treestore_search.foreach(update_doc)

    ############################################################
    # Helper Methods
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

    def _expand_category(self):
        treestore, iter = self.treeview.get_selection().get_selected()
        if iter and treestore.iter_has_child(iter):
            path = treestore.get_path(iter)
            self.treeview.expand_to_path(path)

    ############################################################
    # Event Handlers
    ############################################################
    def _handle_icon_event(self, widget, icon, event):
        if icon == Gtk.EntryIconPosition.PRIMARY:
            pass
        elif icon == Gtk.EntryIconPosition.SECONDARY:
            widget.set_text('')
            self.search_entry.hide()

    def _update_search_tree(self, widget):
        key = widget.get_text().lower()
        if not key:
            self.treeview.set_model(self.treestore)
            self.expand_module_in_tree()
        else:
            matching_blocks = [b for b in list(self.platform.blocks.values())
                               if key in b.key.lower() or key in b.label.lower()]

            self.treestore_search.clear()
            self._categories_search = {tuple(): None}
            for block in matching_blocks:
                self.add_block(block, self.treestore_search,
                               self._categories_search)
            self.treeview.set_model(self.treestore_search)
            self.treeview.expand_all()

    def _handle_search_key_press(self, widget, event):
        """Handle Return and Escape key events in search entry and treeview"""
        if event.keyval == Gdk.KEY_Return:
            # add block on enter

            if widget == self.search_entry:
                #  Get the first block in the search tree and add it
                selected = self.treestore_search.get_iter_first()
                while self.treestore_search.iter_children(selected):
                    selected = self.treestore_search.iter_children(selected)
                if selected is not None:
                    key = self.treestore_search.get_value(selected, KEY_INDEX)
                    if key:
                        self.emit('create_new_block', key)
            elif widget == self.treeview:
                key = self._get_selected_block_key()
                if key:
                    self.emit('create_new_block', key)
                else:
                    self._expand_category()
            else:
                return False  # propagate event

        elif event.keyval == Gdk.KEY_Escape:
            # reset the search
            self.search_entry.set_text('')
            self.search_entry.hide()

        elif (event.get_state() & Gdk.ModifierType.CONTROL_MASK and event.keyval == Gdk.KEY_f) \
                or event.keyval == Gdk.KEY_slash:
            # propagation doesn't work although treeview search is disabled =(
            # manually trigger action...
            Actions.FIND_BLOCKS.activate()

        elif event.get_state() & Gdk.ModifierType.CONTROL_MASK and event.keyval == Gdk.KEY_b:
            # ugly...
            Actions.TOGGLE_BLOCKS_WINDOW.activate()

        else:
            return False  # propagate event

        return True

    def _handle_drag_get_data(self, widget, drag_context, selection_data, info, time):
        """
        Handle a drag and drop by setting the key to the selection object.
        This will call the destination handler for drag and drop.
        Only call set when the key is valid to ignore DND from categories.
        """
        key = self._get_selected_block_key()
        if key:
            selection_data.set_text(key, len(key))

    def _handle_mouse_button_press(self, widget, event):
        """
        Handle the mouse button press.
        If a left double click is detected, call add selected block.
        """
        if event.button == 1 and event.type == Gdk.EventType._2BUTTON_PRESS:
            key = self._get_selected_block_key()
            if key:
                self.emit('create_new_block', key)
