from PyQt5.QtWidgets import QUndoCommand

import logging
from copy import copy

log = logging.getLogger(__name__)

# Movement, rotation, enable/disable/bypass, bus ports,
# change params, toggle type.
# Basically anything that's not cut/paste or new/delete
class ChangeStateAction(QUndoCommand):
    def __init__(self, flowgraph):
        QUndoCommand.__init__(self)
        log.debug("init ChangeState")
        self.oldStates = []
        self.oldParams = []
        self.newStates = []
        self.newParams = []
        self.flowgraph = flowgraph
        self.blocks = flowgraph.selected_blocks()
        for block in self.blocks:
            self.oldStates.append(copy(block.states))
            self.newStates.append(copy(block.states))
            self.oldParams.append(copy(block.params))
            self.newParams.append(copy(block.params))

    def redo(self):
        for i in range(len(self.blocks)):
            self.blocks[i].setStates(self.newStates[i])
            self.blocks[i].params = (self.newParams[i])
        self.flowgraph.update()

    def undo(self):
        for i in range(len(self.blocks)):
            self.blocks[i].setStates(self.oldStates[i])
            self.blocks[i].params = (self.oldParams[i])
        self.flowgraph.update()

class RotateAction(ChangeStateAction):
    def __init__(self, flowgraph, delta_angle):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init RotateAction")
        for states in self.newStates:
            states['rotation'] += delta_angle
            # Get rid of superfluous entries
            states = dict((k, v) for k, v in states.items() if all(k == 'rotation' for x in k))

class MoveAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init MoveAction")
        for i in range(len(self.blocks)):
            self.oldStates[i]['coordinate'] = self.blocks[i].oldPos
            self.newStates[i]['coordinate'] = self.blocks[i].newPos
            self.oldStates[i] = dict((k, v) for k, v in self.oldStates[i].items() if all(k == 'coordinate' for x in k))
            self.newStates[i] = dict((k, v) for k, v in self.newStates[i].items() if all(k == 'coordinate' for x in k))

class EnableAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init EnableAction")
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'enabled'
class DisableAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init DisableAction")
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'disabled'
class BypassAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init BypassAction")
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'bypassed'





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

