# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


try:
    from PyQt5 import Qt, QtCore, QtGui, QtWidgets
except ImportError:
    raise SystemExit(
        'Please install PyQt5 to run this script (http://www.riverbankcomputing.co.uk/software/pyqt/download)')


# Movable solid line for filter ideal-band diagram.
# Enable split to cut the line into two (for bpf).
class filtermovlineItem(QtWidgets.QGraphicsObject):
    attenChanged = QtCore.pyqtSignal(float)

    def __init__(self, x1, y1, x2, y2, lower, upper, split=False, sx1=0, sy1=0, sx2=0, sy2=0):
        QtWidgets.QGraphicsObject.__init__(self)
        self.lower = lower
        self.upper = upper
        self.x1, self.y1 = x1, y1
        self.x2, self.y2 = x2, y2
        self.sx1, self.sy1 = sx1, sy1
        self.sx2, self.sy2 = sx2, sy2
        self.split = split

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.black, 2, QtCore.Qt.SolidLine))
        painter.drawLine(self.x1, self.y1, self.x2, self.y2)
        painter.drawLine(self.x1, self.y1, self.x1, self.y1 - 5)
        painter.drawLine(self.x2, self.y2, self.x2, self.y2 - 5)
        if self.split:
            painter.drawLine(self.sx1, self.sy1, self.sx2, self.sy2)
            painter.drawLine(self.sx1, self.sy1, self.sx1, self.sy1 - 5)
            painter.drawLine(self.sx2, self.sy2, self.sx2, self.sy2 - 5)

    def boundingRect(self):
        return QtCore.QRectF(0, 0, 400, 400)

    # Allow only vertical movement and emit signals.
    def itemChange(self, change, value):
        if (change == QtWidgets.QGraphicsItem.ItemPositionChange):
            newpos = QtCore.QPointF(value)
            div = 0
            if newpos.y() < self.pos().y():
                div = 1
            if newpos.y() > self.pos().y():
                div = -1
            hit = 0
            rect = self.scene().sceneRect()
            if (newpos.y() >= self.lower):
                newpos.setY(self.lower)
                hit = 1
            if (newpos.y() <= self.upper):
                newpos.setY(self.upper)
                hit = 1
            if not(hit):
                self.attenChanged.emit(div)
            return QtCore.QPointF(self.pos().x(), newpos.y())
        return QtWidgets.QGraphicsItem.itemChange(self, change, value)


# Static lines in lpf band diagram.
class lpfsLines(QtWidgets.QGraphicsObject):
    def __init__(self):
        QtWidgets.QGraphicsObject.__init__(self)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.darkGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(5, 20, 5, 200)
        painter.drawLine(5, 200, 400, 200)
        painter.setPen(QtGui.QPen(QtCore.Qt.lightGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(6, 105, 150, 105)
        painter.drawLine(150, 105, 150, 199)
        painter.drawLine(200, 180, 400, 180)
        painter.drawLine(200, 180, 200, 199)
        painter.setPen(QtGui.QPen(QtCore.Qt.black))
        painter.save()
        painter.rotate(270)
        painter.drawText(-150, -10, "Magnitude (dB)")
        painter.restore()
        painter.drawText(350, 220, "Frequency (Hz)")
        painter.drawText(130, 220, "Fpass")
        painter.drawText(180, 220, "Fstop")

    def boundingRect(self):
        return QtCore.QRectF(0, 0, 300, 300)


# Static lines in hpf band diagram.
class hpfsLines(QtWidgets.QGraphicsObject):
    def __init__(self):
        QtWidgets.QGraphicsObject.__init__(self)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.darkGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(5, 20, 5, 200)
        painter.drawLine(5, 200, 400, 200)
        painter.setPen(QtGui.QPen(QtCore.Qt.lightGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(200, 105, 400, 105)
        painter.drawLine(200, 105, 200, 199)
        painter.drawLine(6, 180, 150, 180)
        painter.drawLine(150, 180, 150, 199)
        painter.setPen(QtGui.QPen(QtCore.Qt.black))
        painter.drawText(350, 220, "Frequency (Hz)")
        painter.drawText(130, 220, "Fstop")
        painter.drawText(180, 220, "Fpass")

    def boundingRect(self):
        return QtCore.QRectF(0, 0, 300, 300)


# Static lines in bpf band diagram.
class bpfsLines(QtWidgets.QGraphicsObject):
    def __init__(self):
        QtWidgets.QGraphicsObject.__init__(self)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.darkGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(5, 20, 5, 200)
        painter.drawLine(5, 200, 400, 200)
        painter.setPen(QtGui.QPen(QtCore.Qt.lightGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(6, 180, 110, 180)
        painter.drawLine(110, 180, 110, 199)
        painter.drawLine(155, 105, 255, 105)
        painter.drawLine(255, 105, 255, 199)
        painter.drawLine(155, 105, 155, 199)
        painter.drawLine(300, 180, 400, 180)
        painter.drawLine(300, 180, 300, 199)
        painter.setPen(QtGui.QPen(QtCore.Qt.black))
        painter.drawText(350, 220, "Frequency (Hz)")
        painter.drawText(80, 220, "Fstop1")
        painter.drawText(140, 220, "Fpass1")
        painter.drawText(220, 220, "Fpass2")
        painter.drawText(290, 220, "Fstop2")

    def boundingRect(self):
        return QtCore.QRectF(0, 0, 300, 300)


# Static lines in bnf band diagram.
class bnfsLines(QtWidgets.QGraphicsObject):
    def __init__(self):
        QtWidgets.QGraphicsObject.__init__(self)

    def paint(self, painter, option, widget):
        painter.setPen(QtGui.QPen(QtCore.Qt.darkGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(5, 20, 5, 200)
        painter.drawLine(5, 200, 400, 200)
        painter.setPen(QtGui.QPen(QtCore.Qt.lightGray, 2, QtCore.Qt.SolidLine))
        painter.drawLine(6, 105, 110, 105)
        painter.drawLine(110, 105, 110, 199)
        painter.drawLine(155, 180, 255, 180)
        painter.drawLine(255, 180, 255, 199)
        painter.drawLine(155, 180, 155, 199)
        painter.drawLine(300, 105, 400, 105)
        painter.drawLine(300, 105, 300, 199)
        painter.setPen(QtGui.QPen(QtCore.Qt.black))
        painter.drawText(350, 220, "Frequency (Hz)")
        painter.drawText(80, 220, "Fpass1")
        painter.drawText(140, 220, "Fstop1")
        painter.drawText(220, 220, "Fstop2")
        painter.drawText(290, 220, "Fpass2")

    def boundingRect(self):
        return QtCore.QRectF(0, 0, 300, 300)


lpfItems = []
hpfItems = []
bpfItems = []
bnfItems = []

# lpfitems list.
lpfItems.append(filtermovlineItem(200, 175, 400, 175, 0, -60))
# lpfItems.append(filtermovlineItem(200, 145, 400, 145, 30, -30))
lpfItems[0].setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable | QtWidgets.QGraphicsItem.ItemIsMovable |
                     QtWidgets.QGraphicsItem.ItemSendsGeometryChanges)
lpfItems.append(lpfsLines())

# hpfitems list.
hpfItems.append(filtermovlineItem(6, 175, 150, 175, 0, -60))
hpfItems[0].setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable | QtWidgets.QGraphicsItem.ItemIsMovable |
                     QtWidgets.QGraphicsItem.ItemSendsGeometryChanges)
hpfItems.append(hpfsLines())

# bpfitems list.
bpfItems.append(filtermovlineItem(6, 175, 110, 175,
                0, -60, True, 300, 175, 400, 175))
bpfItems[0].setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable | QtWidgets.QGraphicsItem.ItemIsMovable |
                     QtWidgets.QGraphicsItem.ItemSendsGeometryChanges)
bpfItems.append(bpfsLines())

# bnfitems list.
bnfItems.append(filtermovlineItem(155, 175, 255, 175, 0, -60))
bnfItems[0].setFlags(QtWidgets.QGraphicsItem.ItemIsSelectable | QtWidgets.QGraphicsItem.ItemIsMovable |
                     QtWidgets.QGraphicsItem.ItemSendsGeometryChanges)
bnfItems.append(bnfsLines())
