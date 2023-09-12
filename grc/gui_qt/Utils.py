import cairo

import logging
from pathlib import Path

from qtpy import QtGui, QtCore, QtWidgets, QtSvg

from . import Constants
from .components.canvas.colors import FLOWGRAPH_BACKGROUND_COLOR

log = logging.getLogger(__name__)

def get_rotated_coordinate(coor, rotation):
    """
    Rotate the coordinate by the given rotation.
    Args:
        coor: the coordinate x, y tuple
        rotation: the angle in degrees
    Returns:
        the rotated coordinates
    """
    # handles negative angles
    rotation = (rotation + 360) % 360
    if rotation not in Constants.POSSIBLE_ROTATIONS:
        raise ValueError('unusable rotation angle "%s"'%str(rotation))
    # determine the number of degrees to rotate
    cos_r, sin_r = {
        0: (1, 0), 90: (0, 1), 180: (-1, 0), 270: (0, -1),
    }[rotation]
    x, y = coor
    return x * cos_r + y * sin_r, -x * sin_r + y * cos_r

def make_screenshot(fg_view, file_path, transparent_bg=False):
    if not file_path:
        return
    file_path = Path(file_path)
    if file_path.suffix == ".png":
        rect = fg_view.viewport().rect()

        pixmap = QtGui.QPixmap(rect.size())
        painter = QtGui.QPainter(pixmap)

        fg_view.render(painter, QtCore.QRectF(pixmap.rect()), rect)
        pixmap.save(str(file_path),"PNG")
        painter.end()
    elif file_path.suffix == ".svg":
        rect = fg_view.viewport().rect()

        generator = QtSvg.QSvgGenerator()
        generator.setFileName(str(file_path))
        generator.setSize(rect.size())
        painter = QtGui.QPainter(generator)
        fg_view.render(painter)
        painter.end()
    elif file_path.suffix == ".pdf":
        log.warning("PDF screen capture not implemented")
        return # TODO
