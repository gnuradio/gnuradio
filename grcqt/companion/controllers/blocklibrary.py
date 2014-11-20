# GRC imports
from .. import views, base


NAME_INDEX = 0
KEY_INDEX = 1
DOC_INDEX = 2

class BlockLibrary(base.Controller):
    """ GRC.Controllers.BlockLibrary """

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
        self.app.registerDockWidget(self.view, location=self.gp.window.BLOCK_LIBRARY_DOCK_LOCATION)

        # Register the menus
        #self.app.registerMenu(self.view.menus["library"])

    def load_block_tree(self, block_tree):
        """
        Load a block tree with categories and blocks.
        Step 1: Load all blocks from the xml specification.
        Step 2: Load blocks with builtin category specifications.
        
        Args:
            block_tree: the block tree object
        """
        #recursive function to load categories and blocks
        def load_category(cat_n, parent=None):
            #add this category
            parent = (parent or []) + [cat_n.find('name')]
            block_tree.add_block(parent)
            #recursive call to load sub categories
            map(lambda c: load_category(c, parent), cat_n.findall('cat'))
            #add blocks in this category
            for block_key in cat_n.findall('block'):
                if block_key not in self.get_block_keys():
                    print >> sys.stderr, 'Warning: Block key "%s" not found when loading category tree.' % (block_key)
                    continue
                block = self.get_block(block_key)
                #if it exists, the block's category shall not be overridden by the xml tree
                if not block.get_category():
                    block.set_category(parent)

        # recursively load the category trees and update the categories for each block
        for category_tree_n in self._category_trees_n:
            load_category(category_tree_n)

        #add blocks to block tree
        for block in self.get_blocks():
            #blocks with empty categories are hidden
            if not block.get_category(): continue
            block_tree.add_block(block.get_category(), block)
