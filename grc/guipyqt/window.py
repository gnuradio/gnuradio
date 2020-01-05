import sys
from PyQt5 import QtCore, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QLabel, QGridLayout, QWidget, QTreeView, QAbstractItemView
from PyQt5.QtCore import QSize, QDataStream, QVariant, Qt
from PyQt5.QtGui import QStandardItem, QStandardItemModel
import six
import time

class MyQTreeView(QTreeView):
    def __init__(self):
        QTreeView.__init__(self)

    def dragEnterEvent(self, event):
        print(event.QueryWhatsThis)
        print(event.DragEnter)
        print(event.WhatsThis)
        print(event.WhatsThisClicked)

        if event.mimeData().hasUrls:
            event.accept()
        else:
            event.ignore()
        print("dragEnterEvent")



    def dragMoveEvent(self, event):
        print("dragMoveEvent")
        if event.mimeData().hasUrls:
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
        else:
            event.ignore()


    def dropEvent(self, event):
        QTreeView.dropEvent(self, event)
        if event.mimeData().hasUrls:
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
            # to get a list of files:
            drop_list = []
            for url in event.mimeData().urls():
                drop_list.append(str(url.toLocalFile()))
            # handle the list here
        else:
            event.ignore()







class BlockModel(QStandardItemModel):
    def __init__(self):
        QStandardItemModel.__init__(self)

    def decode_data(self, bytearray):

        data = []
        item = {}

        ds = QDataStream(bytearray)
        while not ds.atEnd():

            row = ds.readInt32()
            column = ds.readInt32()

            map_items = ds.readInt32()
            for i in range(map_items):

                key = ds.readInt32()

                value = QVariant()
                ds >> value
                item[Qt.ItemDataRole(key)] = value

            data.append(item)

        return data

    def itemData(self, x):
        return {0: x.data()}

    def dropMimeData(self, data, action, row, column, parent):
        print(row, column)
        if data.hasFormat('application/x-qabstractitemmodeldatalist'):
            bytearray = data.data('application/x-qabstractitemmodeldatalist')

            data_items = self.decode_data(bytearray)

            # Assuming that we get at least one item, and that it defines text that we can display.
            text = data_items[0][Qt.DisplayRole]
            print(text.value())

            return True
        else:
            return QStandardItemModel.dropMimeData(self, data, action, row, column, parent)

'''
            for row in range(self.rowCount()):
                name = self.item(row, 0).text()

                print(name)


                if name == text:
                    number_item = self.item(row, 1)
                    print(number_item)
                    number = int(number_item.text())
                    print(number)
                    number_item.setText(str(number + 1))
                    break
                #else:
                #    name_item = QStandardItem(text)
                #    number_item = QStandardItem("1")
                #    self.appendRow([name_item, number_item])
'''


class MyWindow(QMainWindow):
    def __init__(self, platform):
        QMainWindow.__init__(self)

        self.platform = platform

        self.setMinimumSize(QSize(640, 480))
        self.setWindowTitle("Hello world - pythonprogramminglanguage.com")

        centralWidget = QWidget()
        self.setCentralWidget(centralWidget)

        # Create QStandardItemModel to represent blocks that will show up in the TreeView
        block_model = BlockModel() #QStandardItemModel()

        # TreeView
        self.tree = MyQTreeView() # MIGHT NEED A self IN THE ARG, I SEE IT EVERYWHERE ELSE BUT NOT SURE WHAT IT DOES
        self.tree.setModel(block_model)
        self.tree.setHeaderHidden(True)
        self.tree.setDragEnabled(True) # needed to be able to click and hold on one of the rows
        self.tree.setDragDropMode(QAbstractItemView.DragDrop) # definitely needed, options are NoDragDrop, DragOnly, DropOnly, DragDrop, InternalMove
        #self.tree.setEnabled(True)

        gridLayout = QGridLayout(centralWidget)
        centralWidget.setLayout(gridLayout)
        gridLayout.addWidget(self.tree, 0, 0)


        # Create tree/dict structure out of list of blocks. Takes ~1 ms on Marc's machine
        block_tree = {}
        for block in six.itervalues(self.platform.blocks):
            if block.category: # blocks with None category should be left out for whatever reason (e.g. not installed)
                #print(block.category) # in list form, e.g. ['Core', 'Digital Television', 'ATSC']
                #print(block.label) # label GRC uses to name block
                #print(block.key) # actual block name (i.e. class name)
                #print('')

                # Add category if it's not in the tree yet
                if block.category[0] not in block_tree:
                    block_tree[block.category[0]] = {}

                if len(block.category) > 1:
                    if block.category[1] not in block_tree[block.category[0]]:
                        block_tree[block.category[0]][block.category[1]] = {}

                if len(block.category) > 2:
                    if block.category[2] not in block_tree[block.category[0]][block.category[1]]:
                        block_tree[block.category[0]][block.category[1]][block.category[2]] = {}

                # Add block to tree
                if len(block.category) == 1:
                    block_tree[block.category[0]][block.label] = {} # By using an empty dict instead of just a string, we keep the dict a pure tree of dicts and nothing else
                elif len(block.category) == 2:
                    block_tree[block.category[0]][block.category[1]][block.label] = {}
                elif len(block.category) == 3:
                    block_tree[block.category[0]][block.category[1]][block.category[2]][block.label] = {}
                else:
                    print("ERROR, SHOULD NOT HAVE THIS MANY CATEGORIES!")


        # Populate TreeView using recursive method. Takes ~1.5 ms on Marc's machine
        self._populateTree(block_tree, block_model.invisibleRootItem())





    # Recursive method of populating the tree
    def _populateTree(self, children, parent):
        for child in sorted(children):
            child_item = QStandardItem(child)
            child_item.setEditable(False)
            child_item.setDragEnabled(True)
            child_item.setDropEnabled(True)
            #child_item.setEnabled(True)
            parent.appendRow(child_item)
            if bool(children): # if dict is not empty it will return True
                self._populateTree(children[child], child_item)


