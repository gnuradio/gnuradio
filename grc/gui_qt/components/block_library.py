# Copyright 2014-2020 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import, print_function

# Standard modules
import logging

# Third-party  modules
import six

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QStandardItemModel

# Custom modules
from .. import base

# Logging
log = logging.getLogger(__name__)


class BlockLibrary(QtWidgets.QDockWidget, base.Component):

    def __init__(self):
        QtWidgets.QDockWidget.__init__(self)
        base.Component.__init__(self)

        self.setObjectName('block_library')
        self.setWindowTitle('Available Blocks')

        # TODO: Pull from preferences and revert to default if not found?
        self.resize(400, 300)
        self.setFloating(False)

        ### GUI Widgets

        # Create the layout widget
        container = QtWidgets.QWidget(self)
        container.setObjectName('block_library::container')
        self._container = container

        layout = QtWidgets.QVBoxLayout(container)
        layout.setObjectName('block_library::layout')
        layout.setSpacing(0)
        layout.setContentsMargins(5, 0, 5, 5)
        self._layout = layout

        # Setup the model for holding block data
        self._model = QtGui.QStandardItemModel()

        library = QtWidgets.QTreeView(container)
        library.setObjectName('block_library::library')
        library.setModel(self._model)
        library.setDragEnabled(True)
        library.setDragDropMode(QtWidgets.QAbstractItemView.DragOnly)
        #library.setColumnCount(1)
        library.setHeaderHidden(True)
        # Expand categories with a single click
        library.clicked.connect(lambda x:
                library.collapse(library.currentIndex())
                if library.isExpanded(library.currentIndex())
                else library.expand(library.currentIndex()))
        #library.headerItem().setText(0, "Blocks")
        self._library = library

        # TODO: Needs to be subclassed
        search_bar = QtWidgets.QLineEdit()
        search_bar.setPlaceholderText("Find a block")
        self._search_bar = search_bar



        # Add widgets to the component
        layout.addWidget(search_bar)
        layout.addSpacing(5)
        layout.addWidget(library)
        container.setLayout(layout)
        self.setWidget(container)

        ### Translation support

        #self.setWindowTitle(_translate("blockLibraryDock", "Library", None))
        #library.headerItem().setText(0, _translate("blockLibraryDock", "Blocks", None))
        #QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)

        ### Loading blocks

        # Keep as a separate function so it can be called at a later point (Reloading blocks)
        self._block_labels = []
        self._block_tree = self.load_blocks()
        self.populate_tree(self._block_tree)

        # TODO: The completer must probably be subclassed to work with the TreeView
        completer = QtWidgets.QCompleter(self._block_labels)
        completer.setCaseSensitivity(QtCore.Qt.CaseInsensitive)
        completer.setFilterMode(QtCore.Qt.MatchContains)
        self._search_bar.setCompleter(completer)

        # TODO: Move to the base controller and set actions as class attributes
        # Automatically create the actions, menus and toolbars.
        # Child controllers need to call the register functions to integrate into the mainwindow
        self.actions = {}
        self.menus = {}
        self.toolbars = {}
        self.createActions(self.actions)
        self.createMenus(self.actions, self.menus)
        self.createToolbars(self.actions, self.toolbars)
        self.connectSlots()

        # Register the dock widget through the AppController.
        # The AppController then tries to find a saved dock location from the preferences
        # before calling the MainWindow Controller to add the widget.
        self.app.registerDockWidget(self, location=self.settings.window.BLOCK_LIBRARY_DOCK_LOCATION)

        # Register the menus
        #self.app.registerMenu(self.menus["library"])


    ### Actions

    def createActions(self, actions):
        log.debug("Creating actions")

        '''
        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                shortcut=Keys.New, statusTip=_("save-tooltip"))

        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                         shortcut=Keys.Open, statusTip=_("clear-tooltip"))
        '''

    def createMenus(self, actions, menus):
        log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        log.debug("Creating toolbars")

    '''

    try:
        _fromUtf8 = QtCore.QString.fromUtf8
    except AttributeError:
        def _fromUtf8(s):
            return s

    try:
        _encoding = QtGui.QApplication.UnicodeUTF8
        def _translate(context, text, disambig):
            return QtGui.QApplication.translate(context, text, disambig, _encoding)
    except AttributeError:
        def _translate(context, text, disambig):
            return QtGui.QApplication.translate(context, text, disambig)
    '''


    def load_blocks(self):
        ''' Load the block tree from the platform and populate the widget. '''
        # Loop through all of the blocks and create the nested hierarchy (this can be unlimited nesting)
        # This takes advantage of Python's use of references to move through the nested layers

        log.info("Loading blocks")
        block_tree = {}
        for block in six.itervalues(self.app.platform.blocks):
            if block.category:
                # Blocks with None category should be left out for whatever reason (e.g. not installed)
                #print(block.category) # in list form, e.g. ['Core', 'Digital Television', 'ATSC']
                #print(block.label) # label GRC uses to name block
                #print(block.key) # actual block name (i.e. class name)

                # Create a copy of the category list so things can be removed without changing the original list
                category = block.category[:]

                # Get a reference to the main block tree.
                # As nested categories are added, this is updated to point to the proper sub-tree in the next layer
                sub_tree = block_tree
                while category:
                    current = category.pop(0)
                    if current not in sub_tree.keys():
                        # Create the new sub-tree
                        sub_tree[current] = {}
                    # Move to the next layer in the block tree
                    sub_tree = sub_tree[current]
                # Sub_tree should now point at the final node of the block_tree that contains the block
                # Add a reference to the block object to the proper subtree
                sub_tree[block.label] = block
                self._block_labels.append(block.label)
        # Save a reference to the block tree in case it is needed later
        self._block_tree = block_tree
        return block_tree

    def populate_tree(self, block_tree):
        ''' Populate the item model and tree view with the hierarchical block tree. '''
        # Recursive method of populating the QStandardItemModel
        # Since the _model.invisibleRootItem is the initial parent, this will populate
        # the model which is used for the TreeView.

        def _populate(blocks, parent):
            for name, obj in sorted(blocks.items()):
                child_item = QtGui.QStandardItem()
                child_item.setEditable(False)
                if type(obj) is dict: # It's a category
                    child_item.setText(name)
                    child_item.setDragEnabled(False) # categories should not be draggable
                    _populate(obj, child_item)
                else: # It's a block
                    child_item.setText(obj.label)
                    child_item.setDragEnabled(True)
                    child_item.setSelectable(True)
                    child_item.setData(QtCore.QVariant(obj.key), role=QtCore.Qt.UserRole,)
                parent.appendRow(child_item)

        # Call the nested function recursively to populate the block tree
        log.debug("Populating the treeview")
        _populate(block_tree, self._model.invisibleRootItem())
