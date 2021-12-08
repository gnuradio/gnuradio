from PyQt5.QtWidgets import QUndoCommand

import logging

log = logging.getLogger(__name__)

class MoveBlockCommand(QUndoCommand):
    def __init__(self):
        pass

class RotateCommand(QUndoCommand):
    def __init__(self, flowgraph, angle):
        # TODO: Keep track of which items are being rotated
        QUndoCommand.__init__(self)
        log.debug("init Rotate")
        self.flowgraph = flowgraph
        self.angle = angle

    def undo(self):
        log.debug("undo Rotate")
        self.flowgraph.rotate_selected(-self.angle)
        self.flowgraph.update()

    def redo(self):
        log.debug("redo Rotate")
        self.flowgraph.rotate_selected(self.angle)
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

