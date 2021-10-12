"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


import math

from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import Qt

from . import colors
from ... import Constants
from ....core.utils.descriptors import nop_write
from ....core.ports import Port as CorePort


class Port(QtWidgets.QGraphicsItem, CorePort):
    """The graphical port."""

    @classmethod
    def make_cls_with_base(cls, super_cls):
        name = super_cls.__name__
        bases = (super_cls,) + cls.__bases__[:-1]
        namespace = cls.__dict__.copy()
        return type(name, bases, namespace)

    def __init__(self, parent, direction, **n):
        """
        Port constructor.
        Create list of connector coordinates.
        """
        self._parent = parent
        super(self.__class__, self).__init__(parent, direction, **n)
        #CorePort.__init__(self, parent, direction, **n)
        QtWidgets.QGraphicsItem.__init__(self)
        self.y_offset = 0
        self.height = 15.0
        self.width = 15.0

        if self._dir == "sink":
            self.connection_point = self.scenePos() + QtCore.QPointF(0.0, self.height / 2.0)
        else:
            self.connection_point = self.scenePos() + QtCore.QPointF(15.0, self.height / 2.0)

        self.setFlag(QtWidgets.QGraphicsItem.ItemSendsScenePositionChanges)

        self._border_color = self._bg_color = colors.BLOCK_ENABLED_COLOR
        self.parent_block.parent.addItem(self)
        self.setFlag(QtWidgets.QGraphicsItem.ItemStacksBehindParent)

    def itemChange(self, change, value):
        if self._dir == "sink":
            self.connection_point = self.scenePos() + QtCore.QPointF(0.0, self.height / 2.0)
        else:
            self.connection_point = self.scenePos() + QtCore.QPointF(15.0, self.height / 2.0)
        for conn in self.connections():
            conn.updateLine()
        return QtWidgets.QGraphicsLineItem.itemChange(self, change, value)

    def create_shapes(self):
        """Create new areas and labels for the port."""
        fm = QtGui.QFontMetrics(QtGui.QFont('Helvetica', 8))
        self.width = max(15, fm.width(self.name) * 1.5)

    def create_labels(self, cr=None):
        """Create the labels for the socket."""
        pass


    def create_shapes_and_labels(self):
        if not self.parentItem():
            self.setParentItem(self.parent_block)
        self.create_shapes()
        self._update_colors()

    def _update_colors(self):
        """
        Get the color that represents this port's type.
        Codes differ for ports where the vec length is 1 or greater than 1.
        Returns:
            a hex color code.
        """
        if not self.parent.enabled:
            #self._font_color[-1] = 0.4
            color = colors.BLOCK_DISABLED_COLOR
        elif self.domain == Constants.GR_MESSAGE_DOMAIN:
            color = colors.PORT_TYPE_TO_COLOR.get('message')
        else:
            #self._font_color[-1] = 1.0
            color = colors.PORT_TYPE_TO_COLOR.get(self.dtype) or colors.PORT_TYPE_TO_COLOR.get('')
            if self.vlen > 1:
                dark = (0, 0, 30 / 255.0, 50 / 255.0, 70 / 255.0)[min(4, self.vlen)]
                #color = tuple(max(c - dark, 0) for c in color)
        self._bg_color = color
        self._border_color = color
        #self._border_color = tuple(max(c - 0.3, 0) for c in color)

    def boundingRect(self):
        x, y = tuple(self.parent.states['coordinate'])
        if self._dir == "sink":
            return QtCore.QRectF(-max(0, self.width - 15), 0, self.width, 15) # same as the rectangle we draw, but with a 0.5*pen width margin
        else:
            return QtCore.QRectF(0, 0, self.width, 15) # same as the rectangle we draw, but with a 0.5*pen width margin

    def paint(self, painter, option, widget):
        """
        Draw the socket with a label.
        """
        if self.hidden:
            return
        painter.setRenderHint(QtGui.QPainter.Antialiasing)

        x, y = tuple(self.parent.states['coordinate'])
        pen = QtGui.QPen(self._border_color)
        painter.setPen(pen)
        painter.setBrush(QtGui.QBrush(self._bg_color))

        if self._dir == "sink":
            rect = QtCore.QRectF(-max(0, self.width - 15), 0, self.width, 15) # same as the rectangle we draw, but with a 0.5*pen width margin
        else:
            rect = QtCore.QRectF(0, 0, self.width, 15) # same as the rectangle we draw, but with a 0.5*pen width margin
        painter.drawRect(rect)

        painter.setPen(QtGui.QPen(1))
        font = QtGui.QFont('Helvetica', 8)
        painter.setFont(font)
        if self._dir == "sink":
            painter.drawText(QtCore.QRectF(-max(0, self.width - 15), 0, self.width, 15), Qt.AlignCenter, self.name)
        else:
            painter.drawText(QtCore.QRectF(0, 0, self.width, 15), Qt.AlignCenter, self.name)
