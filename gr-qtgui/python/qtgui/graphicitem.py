#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from qtpy.QtCore import KeepAspectRatio, QSize
from qtpy.QtWidgets import QLabel
from qtpy.QtGui import QPixmap, QPainter

import os
import sys

from gnuradio import gr
import pmt


class GrGraphicItem(gr.sync_block, QLabel):
    """
    This block displays the selected graphic item.  You can pass a
    filename as a string in a message to change the image on the fly.
    overlays can also be added by passing in a message with a
    dictionary of a list of dictionaries in the car portion of the
    message.  Each dictionary should have the following keys:
    'filename','x','y', and an optional 'scalefactor'.
    Setting the x/y attributes to -1,-1 will remove an overlay.
    Otherwise items are indexed by filename and can be animated
    throughout the background image.
    """

    def __init__(self, image_file, scaleImage=True, fixedSize=False, setWidth=0, setHeight=0):
        gr.sync_block.__init__(self, name="GrGraphicsItem",
                               in_sig=None, out_sig=None)
        QLabel.__init__(self)

        if not os.path.isfile(image_file):
            gr.log.error("ERROR: Unable to find file " + image_file)
            sys.exit(1)

        try:
            self.pixmap = QPixmap(image_file)
            self.originalPixmap = QPixmap(image_file)
        except OSError as e:
            gr.log.error("ERROR: " + e.strerror)
            sys.exit(1)

        self.image_file = image_file
        self.scaleImage = scaleImage
        self.fixedSize = fixedSize
        self.setWidth = setWidth
        self.setHeight = setHeight
        super().setPixmap(self.pixmap)
        super().setMinimumSize(1, 1)
        self.overlays = {}

        self.message_port_register_in(pmt.intern("filename"))
        self.set_msg_handler(pmt.intern("filename"), self.msgHandler)

        self.message_port_register_in(pmt.intern("overlay"))
        self.set_msg_handler(pmt.intern("overlay"), self.overlayHandler)

    def overlayHandler(self, msg):
        try:
            overlayitem = pmt.to_python(pmt.car(msg))
            if overlayitem is None:
                gr.log.error('Overlay message contains None in the car portion '
                             'of the message.  Please pass in a dictionary or list of dictionaries in this '
                             'portion of the message.  Each dictionary should have the following keys: '
                             'filename,x,y.  Use x=y=-1 to remove an overlay item.')
                return

            if type(overlayitem) is dict:
                itemlist = []
                itemlist.append(overlayitem)
            elif type(overlayitem) is list:
                itemlist = overlayitem
            else:
                gr.log.error("Overlay message type is not correct.  Please pass in "
                             "a dictionary or list of dictionaries in this portion of the message.  Each "
                             "dictionary should have the following keys: filename,x,y.  Use x=y=-1 to "
                             "remove an overlay item.")
                return

            # Check each dict item to make sure it's valid.
            for curitem in itemlist:
                if type(curitem) is dict:
                    if 'filename' not in curitem:
                        gr.log.error(
                            "Dictionary item did not contain the 'filename' key.")
                        gr.log.error("Received " + str(curitem))
                        continue

                    if 'x' not in curitem:
                        gr.log.error("The dictionary for filename " +
                                     curitem['filename'] + " did not contain an 'x' key.")
                        gr.log.error("Received " + str(curitem))
                        continue

                    if 'y' not in curitem:
                        gr.log.error("The dictionary for filename " +
                                     curitem['filename'] + " did not contain an 'y' key.")
                        gr.log.error("Received " + str(curitem))
                        continue

                    if not os.path.isfile(curitem['filename']):
                        gr.log.error("Unable to find overlay file " +
                                     curitem['filename'])
                        gr.log.error("Received " + str(curitem))
                        continue

                    # Now either add/update our list or remove the item.
                    if curitem['x'] == -1 and curitem['y'] == -1:
                        try:
                            # remove item
                            del self.overlays[curitem['filename']]
                        except:
                            pass
                    else:
                        self.overlays[curitem['filename']] = curitem

                self.updateGraphic()
        except Exception as e:
            gr.log.error("Error with overlay message conversion: %s" % str(e))

    def updateGraphic(self):
        if (len(self.overlays.keys()) == 0):
            try:
                super().setPixmap(self.pixmap)
            except Exception as e:
                gr.log.error("Error updating graphic: %s" % str(e))
                return
        else:
            # Need to deal with overlays
            tmpPxmap = self.pixmap.copy(self.pixmap.rect())
            painter = QPainter(tmpPxmap)
            for curkey in self.overlays.keys():
                curOverlay = self.overlays[curkey]
                try:
                    newOverlay = QPixmap(curkey)
                    if 'scalefactor' in curOverlay:
                        scale = curOverlay['scalefactor']
                        w = newOverlay.width()
                        h = newOverlay.height()
                        newOverlay = newOverlay.scaled(int(w * scale), int(h * scale),
                                                       KeepAspectRatio)
                    painter.drawPixmap(
                        curOverlay['x'], curOverlay['y'], newOverlay)
                except Exception as e:
                    gr.log.error("Error adding overlay: %s" % str(e))
                    return

            painter.end()

            super().setPixmap(tmpPxmap)

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))
            image_file = new_val
            if type(new_val) is str:
                if not os.path.isfile(image_file):
                    gr.log.error("ERROR: Unable to find file " + image_file)
                    return

                try:
                    self.pixmap = QPixmap(image_file)
                    self.image_file = image_file
                except OSError as e:
                    gr.log.error("ERROR: " + e.strerror)
                    return

                self.updateGraphic()
            else:
                gr.log.error("Value received was not an int or "
                             "a bool: %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def minimumSizeHint(self):
        return QSize(self.pixmap.width(), self.pixmap.height())

    def resizeEvent(self, event):
        if self.scaleImage:
            w = super().width()
            h = super().height()

            self.pixmap = self.originalPixmap.scaled(w, h, KeepAspectRatio)
        elif self.fixedSize and self.setWidth > 0 and self.setHeight > 0:
            self.pixmap = self.originalPixmap.scaled(self.setWidth, self.setHeight,
                                                     KeepAspectRatio)

        self.updateGraphic()
