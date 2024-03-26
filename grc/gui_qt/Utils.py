import numbers
import os
import logging
from pathlib import Path

from qtpy import QtGui, QtCore

from . import Constants

log = logging.getLogger(f"grc.application.{__name__}")


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
        raise ValueError('unusable rotation angle "%s"' % str(rotation))
    # determine the number of degrees to rotate
    cos_r, sin_r = {
        0: (1, 0),
        90: (0, 1),
        180: (-1, 0),
        270: (0, -1),
    }[rotation]
    x, y = coor
    return x * cos_r + y * sin_r, -x * sin_r + y * cos_r


def num_to_str(num):
    """ Display logic for numbers """
    def eng_notation(value, fmt='g'):
        """Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n"""
        template = '{:' + fmt + '}{}'
        magnitude = abs(value)
        for exp, symbol in zip(range(9, -15 - 1, -3), 'GMk munpf'):
            factor = 10 ** exp
            if magnitude >= factor:
                return template.format(value / factor, symbol.strip())
        return template.format(value, '')

    if isinstance(num, numbers.Complex):
        num = complex(num)  # Cast to python complex
        if num == 0:
            return '0'
        output = eng_notation(num.real) if num.real else ''
        output += eng_notation(num.imag, '+g' if output else 'g') + \
            'j' if num.imag else ''
        return output
    else:
        return str(num)


_nproc = None


def get_cmake_nproc():
    """ Get number of cmake processes for C++ flowgraphs """
    global _nproc  # Cached result
    if _nproc:
        return _nproc
    try:
        # See https://docs.python.org/3.8/library/os.html#os.cpu_count
        _nproc = len(os.sched_getaffinity(0))
    except:
        _nproc = os.cpu_count()
    if not _nproc:
        _nproc = 1

    _nproc = max(_nproc // 2 - 1, 1)
    return _nproc


def make_screenshot(fg_view, file_path, transparent_bg=False):
    if not file_path:
        return
    file_path = Path(file_path)
    if file_path.suffix == ".png":
        rect = fg_view.viewport().rect()

        pixmap = QtGui.QPixmap(rect.size())
        painter = QtGui.QPainter(pixmap)

        fg_view.render(painter, QtCore.QRectF(pixmap.rect()), rect)
        pixmap.save(str(file_path), "PNG")
        painter.end()
    elif file_path.suffix == ".svg":
        try:
            from qtpy import QtSvg
        except ImportError:
            log.error("Missing (Python-)QtSvg! Please install it or export as PNG instead.")
            return
        rect = fg_view.viewport().rect()

        generator = QtSvg.QSvgGenerator()
        generator.setFileName(str(file_path))
        generator.setSize(rect.size())
        painter = QtGui.QPainter(generator)
        fg_view.render(painter)
        painter.end()
    elif file_path.suffix == ".pdf":
        log.warning("PDF screen capture not implemented")
        return  # TODO
