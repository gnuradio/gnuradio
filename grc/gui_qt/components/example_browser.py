import logging
import os
import traceback

from qtpy import uic
from qtpy.QtCore import QObject, Signal, Slot, QRunnable, QVariant, Qt
from qtpy.QtGui import QPixmap
from qtpy.QtWidgets import QDialog, QListWidgetItem

from ...core.cache import Cache
from .. import base, Constants
from ..properties import Paths

# Logging
log = logging.getLogger(__name__)


class WorkerSignals(QObject):
    error = Signal(tuple)
    result = Signal(object)
    progress = Signal(tuple)


class Worker(QRunnable):
    """
    This is the Worker that will gather/parse examples as a background task
    """
    def __init__(self, fn, *args, **kwargs):
        super(Worker, self).__init__()

        self.fn = fn
        self.args = args
        self.kwargs = kwargs
        self.signals = WorkerSignals()

        self.kwargs['progress_callback'] = self.signals.progress

    @Slot()
    def run(self):
        try:
            result = self.fn(*self.args, **self.kwargs)
        except:
            print("Error in background task:")
            traceback.print_exc()
        else:
            self.signals.result.emit(result)


class ExampleBrowser(QDialog, base.Component):
    file_to_open = Signal(str)
    data_role = Qt.UserRole

    def __init__(self):
        super().__init__()
        uic.loadUi(Paths.RESOURCES + "/example_browser.ui", self)

        self.cpp_qt_fg = QPixmap(Paths.RESOURCES + "/cpp_qt_fg.png")
        self.cpp_cmd_fg = QPixmap(Paths.RESOURCES + "/cpp_cmd_fg.png")
        self.py_qt_fg = QPixmap(Paths.RESOURCES + "/py_qt_fg.png")
        self.py_cmd_fg = QPixmap(Paths.RESOURCES + "/py_cmd_fg.png")

        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Examples")

        self.examples = self.platform.examples
        self.modules = []
        self.current_module = ""

        self.left_list.currentItemChanged.connect(self.update_mid_list)
        self.mid_list.currentItemChanged.connect(self.populate_preview)
        self.mid_list.itemDoubleClicked.connect(self.open_file)

        self.open_button.clicked.connect(self.open_file)
        self.close_button.clicked.connect(self.reject)

    def populate(self, examples):
        self.examples = examples

        for ex in self.examples:
            if ex["module"] not in self.modules:
                self.modules.append(ex["module"])
                self.left_list.addItem(ex["module"])
            item = QListWidgetItem()
            item.setText(ex["name"])
            item.setData(self.data_role, QVariant(ex))
            self.mid_list.addItem(item)

        self.left_list.sortItems()
        self.left_list.setCurrentRow(0)
        self.update_mid_list()

    def update_mid_list(self):
        first = False
        for i in range(self.mid_list.count()):
            item = self.mid_list.item(i)

            if item.data(self.data_role)["module"] == self.left_list.currentItem().text():
                if not first:
                    self.mid_list.setCurrentRow(i)
                    first = True
                item.setHidden(False)
            else:
                item.setHidden(True)

    def populate_preview(self):
        ex = self.mid_list.currentItem().data(self.data_role)

        self.title_label.setText(f"<b>Title:</b> {ex['title']}")
        self.author_label.setText(f"<b>Author:</b> {ex['author']}")
        self.desc_label.setText(ex["desc"])

        if ex["output_language"] == "python":
            if ex["generate_options"] == "qt_gui":
                self.image_label.setPixmap(self.py_qt_fg)
            else:
                self.image_label.setPixmap(self.py_cmd_fg)
        else:
            if ex["generate_options"] == "qt_gui":
                self.image_label.setPixmap(self.cpp_qt_fg)
            else:
                self.image_label.setPixmap(self.cpp_cmd_fg)

    def open_file(self):
        ex = self.mid_list.currentItem().data(self.data_role)

        self.file_to_open.emit(ex["path"])
        self.done(0)

    def filter_(self, path_filter: list[str]):
        """
        Only display examples that contain a specific block. (Hide the others)

            Parameters:
                path_filter: A list
        """
        # First, hide all the categories
        for i in range(self.left_list.count()):
            self.left_list.item(i).setHidden(True)

        for i in range(self.mid_list.count()):
            m_item = self.mid_list.item(i)
            if m_item.data(self.data_role)["path"] in path_filter:
                m_item.setHidden(False)
                # Show the example, but also show the category (below)
                for j in range(self.left_list.count()):
                    l_item = self.left_list.item(j)
                    if l_item.text() == m_item.data(self.data_role)["module"]:
                        l_item.setHidden(False)

            m_item.setHidden(True)

        self.left_list.sortItems()
        self.left_list.setCurrentRow(0)
        self.update_mid_list()

    def remove_filter(self):
        """Reset the filter."""
        for i in range(self.left_list.count()):
            self.left_list.item(i).setHidden(False)
        for i in range(self.mid_list.count()):
            self.mid_list.item(i).setHidden(False)

        self.left_list.sortItems()
        self.left_list.setCurrentRow(0)
        self.update_mid_list()

    def find_examples(self, progress_callback, ext="grc"):
        """Iterate through the example flowgraph directories and parse them."""
        examples = []
        with Cache(Constants.EXAMPLE_CACHE_FILE) as cache:
            for entry in self.platform.config.example_paths:
                if entry == '':
                    log.error("Empty example path!")
                if os.path.isdir(entry):
                    subdirs = 0
                    current_subdir = 0
                    for dirpath, dirnames, filenames in os.walk(entry):
                        subdirs += 1  # Loop through once to see how many there are
                    for dirpath, dirnames, filenames in os.walk(entry):
                        current_subdir += 1
                        progress_callback.emit((int(100 * current_subdir / subdirs), "Indexing examples"))
                        for filename in sorted(filter(lambda f: f.endswith('.' + ext), filenames)):
                            file_path = os.path.join(dirpath, filename)
                            try:
                                data = cache.get_or_load(file_path)
                                example = {}
                                example["name"] = os.path.basename(file_path)
                                example["generate_options"] = data["options"]["parameters"].get("generate_options") or "no_gui"
                                example["output_language"] = data["options"]["parameters"].get("output_language") or "python"
                                example["example_filter"] = data["metadata"].get("example_filter") or []
                                example["title"] = data["options"]["parameters"]["title"] or "TITLE"
                                example["desc"] = data["options"]["parameters"]["description"] or "DESCRIPTION"
                                example["author"] = data["options"]["parameters"]["author"] or "AUTHOR"
                                example["path"] = file_path
                                example["module"] = os.path.dirname(file_path).replace(entry, "")
                                if example["module"].startswith("/"):
                                    example["module"] = example["module"][1:]

                                example["blocks"] = set()
                                for block in data["blocks"]:
                                    example["blocks"].add(block["id"])
                                examples.append(example)
                            except Exception:
                                continue

        examples_w_block: dict[str, set[str]] = {}
        designated_examples_w_block: dict[str, set[str]] = {}
        for example in examples:
            if example["example_filter"]:
                for block in example["example_filter"]:
                    try:
                        designated_examples_w_block[block].append(example["path"])
                    except KeyError:
                        designated_examples_w_block[block] = [example["path"]]
                continue
            else:
                for block in example["blocks"]:
                    try:
                        examples_w_block[block].append(example["path"])
                    except KeyError:
                        examples_w_block[block] = [example["path"]]

        return (examples, examples_w_block, designated_examples_w_block)
