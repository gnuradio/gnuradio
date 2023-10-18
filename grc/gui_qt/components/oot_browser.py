from __future__ import absolute_import, print_function

# Standard modules
import logging
import os
import sys
import subprocess
import yaml
import traceback

# Third-party  modules
import six

from qtpy import QtCore, QtGui, QtWidgets, uic
from qtpy.QtCore import Qt, QSettings
from qtpy.QtGui import QStandardItemModel

from .. import base
from ..properties import Paths


# Logging
log = logging.getLogger(__name__)


class OOTBrowser(QtWidgets.QDialog, base.Component):
    data_role = QtCore.Qt.UserRole

    def __init__(self):
        super().__init__()
        uic.loadUi(Paths.RESOURCES + "/oot_browser.ui", self)

        self.setMinimumSize(600, 450)
        self.setModal(True)

        self.setWindowTitle("GRC OOT Modules")

        self.left_list.currentItemChanged.connect(self.populate_right_view)
        mock_module1 = {
            "title": "(mock) The IRIDIUM OOT Module",
            "version": "v1.2.3",
            "brief": "GNU Radio components to receive and demodulate Iridium frames",
            "tags": ["sdr", "iridium"],
            "author": ["schneider  &lt;schneider@muc.ccc.de&gt;"],
            "repo": '<a href="https://github.com/muccc/gr-iridium">https://github.com/muccc/gr-iridium</a>',
            "dependencies": [],
            "copyright_owner": "Free Software Foundation, Inc",
            "license": "GPLv3",
            "gr_supported_version": ["v3.7", "v3.8", "v3.9", "v3.10"],
            "website": "",
            "description": "This module provides blocks to build an Iridium burst detector and demodulator.\n\nIt provides a sample application which can be used to detect and demodulate\ndata from the Iridium satellite network.\n\nYou should also have a look at the [iridium-toolkit](https://github.com/muccc/iridium-toolkit).\n\nMore information about installing and using gr-iridium can be found in\nthe [readme](https://github.com/muccc/gr-iridium/blob/master/README.md).\n\n"
        }
        mock_module2 = {
            "title": "(mock) gr-satellites",
            "version": "v1.2.3",
            "brief": "A collection of decoders for Amateur satellites",
            "tags": ["satellites", "amateur", "ccsds", "fec"],
            "author": ["Daniel Estévez &lt;daniel@destevez.net&gt;"],
            "repo": '<a href="https://github.com/daniestevez/gr-satellites">https://github.com/daniestevez/gr-satellites</a>',
            "dependencies": [],
            "copyright_owner": "Daniel Estévez",
            "license": "GPLv3",
            "gr_supported_version": ["v3.8", "v3.9", "v3.10"],
            "website": "",
            "description": "gr-satellites is an OOT module encompassing a collection of telemetry decoders that supports many different Amateur satellites. This open-source project started in 2015 with the goal of providing telemetry decoders for all the satellites that transmit on the Amateur radio bands. It suports most popular protocols, such as AX.25, the GOMspace NanoCom U482C and AX100 modems, an important part of the CCSDS stack, the AO-40 protocol used in the FUNcube satellites, and several ad-hoc protocols used in other satellites."
        }

        item1 = QtWidgets.QListWidgetItem()
        item1.setText("gr-iridium")
        item1.setData(self.data_role, mock_module1)

        item2 = QtWidgets.QListWidgetItem()
        item2.setText("gr-satellites")
        item2.setData(self.data_role, mock_module2)

        self.left_list.addItem(item1)
        self.left_list.addItem(item2)

    def populate_right_view(self):
        module = self.left_list.currentItem().data(self.data_role)

        self.title_label.setText(f"{module['title']} {module['version']}")
        self.brief_label.setText(module["brief"])
        self.website_label.setText(f"<b>Website:</b> {module['website']}" if module["website"] else "")
        self.dep_label.setText(f"<b>Dependencies:</b> '; '.join({module['dependencies']})" if module["dependencies"] else "<b>Dependencies:</b> None")
        self.repo_label.setText(f"<b>Repository:</b> {module['repo']}")
        self.copyright_label.setText(f"<b>Copyright Owner:</b> {module['copyright_owner']}")
        self.supp_ver_label.setText(f"<b>Supported GNU Radio Versions:</b> {'; '.join(module['gr_supported_version'])}")
        self.tags_label.setText(f"<b>Tags:</b> {'; '.join(module['tags'])}")
        self.license_label.setText(f"<b>License:</b> {module['license']}")
        self.desc_label.setText("\n" + module["description"].replace("\t", ""))
        self.author_label.setText(f"<b>Author(s):</b> {'; '.join(module['author'])}")
