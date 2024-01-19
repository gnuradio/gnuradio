from qtpy.QtWidgets import QUndoCommand

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
        self.setText('Rotate')
        for states in self.newStates:
            states['rotation'] += delta_angle
            # Get rid of superfluous entries
            states = dict((k, v) for k, v in states.items() if all(k == 'rotation' for x in k))
        self.flowgraph.update()


class MoveAction(QUndoCommand):
    def __init__(self, flowgraph, diff):
        QUndoCommand.__init__(self)
        log.debug("init MoveAction")
        self.setText('Move')
        self.blocks = flowgraph.selected_blocks()
        self.flowgraph = flowgraph
        self.x = diff.x()
        self.y = diff.y()
        self.first = True

    # redo() is called when the MoveAction is first created.
    # At this point, the item is already at the correct position.
    # Therefore, do nothing.
    def redo(self):
        if self.first:
            self.first = False
            return
        for block in self.blocks:
            block.moveBy(self.x, self.y)
        self.flowgraph.update()

    def undo(self):
        for block in self.blocks:
            block.moveBy(-self.x, -self.y)
        self.flowgraph.update()


class EnableAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init EnableAction")
        self.setText('Enable')
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'enabled'


class DisableAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init DisableAction")
        self.setText('Disable')
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'disabled'


class BypassAction(ChangeStateAction):
    def __init__(self, flowgraph):
        ChangeStateAction.__init__(self, flowgraph)
        log.debug("init BypassAction")
        self.setText('Bypass')
        for i in range(len(self.blocks)):
            self.newStates[i]['state'] = 'bypassed'


# Change properties
# This can only be performed on one block at a time
class BlockPropsChangeAction(QUndoCommand):
    def __init__(self, flowgraph, block):
        QUndoCommand.__init__(self)
        log.debug("init BlockPropsChangeAction")
        self.setText(f'{block.name} block: Change properties')
        self.flowgraph = flowgraph
        self.block = block
        self.oldData = copy(block.oldData)
        self.newData = copy(block.export_data())
        self.first = True

    def redo(self):
        if self.first:
            self.first = False
            return
        try:
            name = self.newData['name']
        except KeyError:
            name = self.newData['parameters']['id']

        self.block.import_data(name, self.newData['states'], self.newData['parameters'])
        self.block.rewrite()
        self.block.validate()
        self.block.create_shapes_and_labels()
        self.flowgraph.update()

    def undo(self):
        try:
            name = self.oldData['name']
        except KeyError:
            name = self.oldData['parameters']['id']

        self.block.import_data(name, self.oldData['states'], self.oldData['parameters'])
        self.block.rewrite()
        self.block.validate()
        self.block.create_shapes_and_labels()
        self.flowgraph.update()


class BussifyAction(QUndoCommand):
    def __init__(self, flowgraph, direction):
        QUndoCommand.__init__(self)
        log.debug("init BussifyAction")
        self.setText(f'Toggle bus {direction}')
        self.flowgraph = flowgraph
        self.direction = direction
        self.blocks = flowgraph.selected_blocks()

    def bussify(self):
        for block in self.blocks:
            block.bussify(self.direction)
        self.flowgraph.update()

    def redo(self):
        self.bussify()

    def undo(self):
        self.bussify()


# Blocks and connections
class NewElementAction(QUndoCommand):
    def __init__(self, flowgraph, element):
        QUndoCommand.__init__(self)
        log.debug("init NewElementAction")
        self.setText('New')
        self.flowgraph = flowgraph
        self.element = element
        self.first = True

    def redo(self):
        if self.first:
            self.first = False
            return
        self.flowgraph.add_element(self.element)
        self.flowgraph.update()

    def undo(self):
        self.flowgraph.remove_element(self.element)
        self.flowgraph.update()


class DeleteElementAction(QUndoCommand):
    def __init__(self, flowgraph):
        QUndoCommand.__init__(self)
        log.debug("init DeleteElementAction")
        self.setText('Delete')
        self.flowgraph = flowgraph
        self.connections = flowgraph.selected_connections()
        self.blocks = flowgraph.selected_blocks()
        for block in self.blocks:
            self.connections = self.connections + block.connections()

    def redo(self):
        for con in self.connections:
            self.flowgraph.remove_element(con)
        for block in self.blocks:
            self.flowgraph.remove_element(block)
        self.flowgraph.update()

    def undo(self):
        for block in self.blocks:
            self.flowgraph.add_element(block)
        for con in self.connections:
            self.flowgraph.add_element(con)
        self.flowgraph.update()
