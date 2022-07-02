from PyQt5.QtWidgets import QUndoCommand

import logging

log = logging.getLogger(__name__)

# Movement, rotation
class ChangeStateCommand(QUndoCommand):
    def __init__(self, flowgraph, blocks):
        QUndoCommand.__init__(self)
        log.debug("init ChangeState")
        self.flowgraph = flowgraph
        self.blocks = blocks
        self.oldStates = [block.fromStates for block in blocks]
        self.newStates = [block.toStates for block in blocks]

    def redo(self):
        log.debug("redo ChangeState")
        for i in range(len(self.blocks)):
            self.blocks[i].setStates(self.newStates[i])
        self.flowgraph.update()

    def undo(self):
        log.debug("undo ChangeState")
        for i in range(len(self.blocks)):
            self.blocks[i].setStates(self.oldStates[i])
        self.flowgraph.update()

# Blocks and connections
class NewElementCommand(QUndoCommand):
    def __init__(self):
        pass

# Blocks and connections
class DeleteElementCommand(QUndoCommand):
    def __init__(self):
        pass

# Enable, disable, bypass, change properties
class ChangeBlockCommand(QUndoCommand):
    def __init__(self):
        pass

class ToggleBusCommand(QUndoCommand):
    def __init__(self):
        pass

