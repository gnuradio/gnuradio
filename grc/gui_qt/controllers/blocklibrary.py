import six

from PyQt5.QtGui import QStandardItemModel

# GRC imports
from .. import views, base




NAME_INDEX = 0
KEY_INDEX = 1
DOC_INDEX = 2

class BlockLibrary(base.Controller):
    ''' GRC.Controllers.BlockLibrary '''

    def __init__(self):
        # Required function calls
        super().__init__()
        self.setView(views.BlockLibrary)

        # Do other initialization stuff. View should already be allocated and
        # actions dynamically connected to class functions. Also, the self.log
        # functionality should be also allocated
        self.log.debug("__init__")
        self.log.debug("Registering dock")
        # Register the dock widget through the AppController.
        # The AppController then tries to find a saved dock location from the preferences
        # before calling the MainWindow Controller to add the widget.
        self.app.registerDockWidget(self.view, location=self.settings.window.BLOCK_LIBRARY_DOCK_LOCATION)

        # Register the menus
        #self.app.registerMenu(self.view.menus["library"])
        block_tree = self.load_tree()
        self.view.populateTree(block_tree)

    def load_tree(self):
        ''' Load the block tree from the platform and builds a nested dictionary '''

        block_tree = {}
        # Loop through all of the blocks and create the nested hierarchy (this can be unlimited nesting)
        # This takes advantage of Python's use of references to move through the nested layers
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
        return block_tree
