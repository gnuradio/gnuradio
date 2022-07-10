from PyQt5.QtWidgets import QUndoCommand

import logging
from copy import copy

log = logging.getLogger(__name__)

# Movement, rotation, enable/disable/bypass, bus ports
class ChangeStateAction(QUndoCommand):
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

class RotateAction(ChangeStateAction):
    def __init__(self, flowgraph, delta_angle):
        QUndoCommand.__init__(self)
        log.debug("init ChangeState")
        self.oldStates = []
        self.newStates = []
        self.flowgraph = flowgraph
        self.blocks = flowgraph.selected_blocks()
        for block in self.blocks:
            #print(block.states
            self.oldStates.append(copy(block.states))
            self.newStates.append(copy(block.states))
            self.newStates[-1]['rotation'] += delta_angle

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

