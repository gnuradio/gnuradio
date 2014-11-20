from PyQt5 import QtCore, QtGui, QtWidgets

#from PyQt4.QtCore import Qt, QRectF


class DrawingView(QGraphicsView):
    def __init__(self, parent, flow_graph):
        QGraphicsView.__init__(self, flow_graph, parent)
        self._flow_graph = flow_graph

        self.setFrameShape(QFrame.NoFrame)
        self.setRenderHints(QPainter.Antialiasing |
                            QPainter.SmoothPixmapTransform)
        self.setAcceptDrops(True)
        self.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        self.setSceneRect(0, 0, self.width(), self.height())

        self._dragged_block = None

        #ToDo: Better put this in Block()
        #self.setContextMenuPolicy(Qt.ActionsContextMenu)
        #self.addActions(parent.main_window.menuEdit.actions())

    def wheelEvent(self, event):
        if event.modifiers() == Qt.ControlModifier:
            factor = 1.2
            if event.delta() < 0 :
                factor = 1.0 / factor
            self.scale(factor, factor)
        else:
            QGraphicsView.wheelEvent(self, event)

    def dragEnterEvent(self, event):
        key = event.mimeData().text()
        self._dragged_block = self._flow_graph.add_new_block(
            str(key), self.mapToScene(event.pos()))
        event.accept()

    def dragMoveEvent(self, event):
        if self._dragged_block:
            self._dragged_block.setPos(self.mapToScene(event.pos()))
            event.accept()
        else:
            event.ignore()

    def dragLeaveEvent(self, event):
        if self._dragged_block:
            self._flow_graph.remove_element(self._dragged_block)
            self._flow_graph.removeItem(self._dragged_block)

    def dropEvent(self, event):
        self._dragged_block = None
        event.accept()

