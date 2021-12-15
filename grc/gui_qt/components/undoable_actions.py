from PyQt5.QtWidgets import QUndoCommand

import logging

log = logging.getLogger(__name__)

class MoveCommand(QUndoCommand):
    def __init__(self, flowgraph, block):
        QUndoCommand.__init__(self)
        log.debug("init Move")
        self.flowgraph = flowgraph
        self.block = block
        self.oldPos = block.movingFrom
        self.newPos = block.movingTo

    def redo(self):
        log.debug("redo Move")
        self.block.setPos(self.newPos)
        self.flowgraph.update()

    def undo(self):
        log.debug("undo Move")
        self.block.setPos(self.oldPos)
        self.flowgraph.update()

class RotateCommand(QUndoCommand):
    def __init__(self, flowgraph, angle):
        # TODO: Keep track of which items are being rotated
        QUndoCommand.__init__(self)
        log.debug("init Rotate")
        self.flowgraph = flowgraph
        self.angle = angle

    def redo(self):
        log.debug("redo Rotate")
        self.flowgraph.rotate_selected(self.angle)
        self.flowgraph.update()

    def undo(self):
        log.debug("undo Rotate")
        self.flowgraph.rotate_selected(-self.angle)
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

