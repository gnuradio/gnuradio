from PyQt5.QtWidgets import QUndoCommand

class MoveBlockCommand(QUndoCommand):
    def __init__(self):
        pass

class RotateBlockCommand(QUndoCommand):
    def __init__(self):
        pass

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

