from __future__ import absolute_import, print_function

# Standard modules
import logging
import os
import yaml

from qtpy import QtCore, QtWidgets, uic

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

        self.manifest_dir = os.path.join(Paths.RESOURCES, "manifests")

        for f in os.listdir(self.manifest_dir):
            with open(os.path.join(self.manifest_dir, f), 'r', encoding='utf8') as manifest:
                text = manifest.read()
                yml, desc = text.split("---")
                data = yaml.safe_load(yml)
                data["description"] = desc
                item = QtWidgets.QListWidgetItem()
                item.setText(data["title"])
                item.setData(self.data_role, data)

                self.left_list.addItem(item)

        self.left_list.setCurrentRow(0)

    def populate_right_view(self):
        module = self.left_list.currentItem().data(self.data_role)

        self.title_label.setText(f"{module['title']} {module.get('version')}")
        self.brief_label.setText(module["brief"])
        self.website_label.setText(f"<b>Website:</b> {module.get('website')}")
        if module.get("dependencies"):
            self.dep_label.setText(f"<b>Dependencies:</b> {'; '.join(module.get('dependencies'))}")
        else:
            self.dep_label.setText("<b>Dependencies:</b> None")
        self.repo_label.setText(f"<b>Repository:</b> {module.get('repo')}")
        if module.get("copyright_owner"):
            self.copyright_label.setText(f"<b>Copyright Owner:</b> {'; '.join(module['copyright_owner'])}")
        else:
            self.copyright_label.setText("<b>Copyright Owner:</b> None")
        self.supp_ver_label.setText(f"<b>Supported GNU Radio Versions:</b> {'; '.join(module['gr_supported_version'])}")
        self.tags_label.setText(f"<b>Tags:</b> {'; '.join(module['tags'])}")
        self.license_label.setText(f"<b>License:</b> {module['license']}")
        self.desc_label.setMarkdown("\n" + module["description"].replace("\t", ""))
        self.author_label.setText(f"<b>Author(s):</b> {'; '.join(module['author'])}")
