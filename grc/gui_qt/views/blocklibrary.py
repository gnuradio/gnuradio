from PyQt5 import QtCore, QtGui, QtWidgets


# GRC imports
from .. import base


class BlockLibrary(QtWidgets.QDockWidget, base.View):
    ''' GRC.Views.BlockLibrary '''

    def __init__(self):
        super().__init__()
        self.log.debug("__init__")
        self.setObjectName("blocklibrary")
        self.setWindowTitle("Block Library")

        self.resize(400, 300)
        self.setFloating(False)

        contents = QtWidgets.QWidget()
        contents.setObjectName("blocklibrary::contents")

        layout = QtWidgets.QVBoxLayout(contents)
        layout.setSpacing(0)
        layout.setContentsMargins(5, 0, 5, 5)
        layout.setObjectName("blocklibrary::contents::layout")

        self._model = QtGui.QStandardItemModel()
        library = QtWidgets.QTreeView(self)
        library.setModel(self._model)
        library.setDragEnabled(True)
        library.setDragDropMode(QtWidgets.QAbstractItemView.DragOnly)
        #library.setColumnCount(1)
        library.setObjectName("blocklibrary::contents::blocktree")
        library.setHeaderHidden(True)
        #library.header().setVisible(False)
        #library.headerItem().setText(0, "Blocks")
        self._library = library

        layout.addWidget(library)
        self.setWidget(contents)

        #self.setWindowTitle(_translate("blockLibraryDock", "Library", None))
        #library.headerItem().setText(0, _translate("blockLibraryDock", "Blocks", None))
        #QtCore.QMetaObject.connectSlotsByName(blockLibraryDock)

    def createActions(self, actions):
        self.log.debug("Creating actions")

        '''
        # File Actions
        actions['save'] = Action(Icons("document-save"), _("save"), self,
                                shortcut=Keys.New, statusTip=_("save-tooltip"))

        actions['clear'] = Action(Icons("document-close"), _("clear"), self,
                                         shortcut=Keys.Open, statusTip=_("clear-tooltip"))
        '''

    def createMenus(self, actions, menus):
        self.log.debug("Creating menus")

    def createToolbars(self, actions, toolbars):
        self.log.debug("Creating toolbars")


    def populateTree(self, blocks):
        # Recursive method of populating the tree
        def _populate(blocks, parent):
            for name, obj in sorted(blocks.items()):
                child_item = QtGui.QStandardItem()
                child_item.setEditable(False)
                if type(obj) is dict:
                    child_item.setText(name)
                    child_item.setDragEnabled(False) # categories should not be draggable
                    _populate(obj, child_item)
                else:
                    child_item.setText(obj.label)
                    child_item.setDragEnabled(True)
                    child_item.setSelectable(True)
                    child_item.setData(QtCore.QVariant(obj.key), role=QtCore.Qt.UserRole,)
                parent.appendRow(child_item)
        # Call the nested function recursively to populate the block tree
        _populate(blocks, self._model.invisibleRootItem())
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
