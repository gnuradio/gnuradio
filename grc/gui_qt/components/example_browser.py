import logging
import os
import traceback

from qtpy import uic
from qtpy.QtCore import QObject, Signal, Slot, QRunnable, QVariant, Qt
from qtpy.QtGui import QPixmap, QStandardItem, QStandardItemModel
from qtpy.QtWidgets import QDialog, QListWidgetItem, QTreeWidgetItem, QWidget, QVBoxLayout


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


class ExampleBrowserDialog(QDialog):
    def __init__(self, browser):
        super(ExampleBrowserDialog, self).__init__()

        self.setMinimumSize(600, 400)
        self.setModal(True)

        self.setWindowTitle("GRC Examples")
        self.browser = browser
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)
        self.layout.addWidget(browser)
        self.browser.connect_dialog(self)


class ExampleBrowser(QWidget, base.Component):
    file_to_open = Signal(str)
    data_role = Qt.UserRole

    lang_dict = {
        'python': 'Python',
        'cpp': 'C++',
    }

    gen_opts_dict = {
        'no_gui': 'No GUI',
        'qt_gui': 'Qt GUI',
        'bokeh_gui': 'Bokeh GUI',
        'hb': 'Hier block ',
        'hb_qt_gui': 'Hier block (Qt GUI)'
    }

    def __init__(self):
        super(ExampleBrowser, self).__init__()
        uic.loadUi(Paths.RESOURCES + "/example_browser_widget.ui", self)
        self.library = None
        self.dialog = None

        self.tree_widget.setHeaderHidden(True)
        self.tree_widget.clicked.connect(self.handle_clicked)

        self.cpp_qt_fg = QPixmap(Paths.RESOURCES + "/cpp_qt_fg.png")
        self.cpp_cmd_fg = QPixmap(Paths.RESOURCES + "/cpp_cmd_fg.png")
        self.py_qt_fg = QPixmap(Paths.RESOURCES + "/py_qt_fg.png")
        self.py_cmd_fg = QPixmap(Paths.RESOURCES + "/py_cmd_fg.png")

        self.examples_dict = self.platform.examples_dict
        self.dir_items = {}

        self.tree_widget.currentItemChanged.connect(self.populate_preview)
        self.tree_widget.itemDoubleClicked.connect(self.open_file)
        self.open_button.clicked.connect(self.open_file)

    def set_library(self, library):
        self.library = library

    def handle_clicked(self):
        if self.tree_widget.isExpanded(self.tree_widget.currentIndex()):
            self.tree_widget.collapse(self.tree_widget.currentIndex())
        else:
            self.tree_widget.expand(self.tree_widget.currentIndex())

    def connect_dialog(self, dialog: QDialog):
        if self.dialog:
            pass # disconnect?

        self.dialog = dialog
        if isinstance(dialog, ExampleBrowserDialog):
            self.close_button.setHidden(False)
            self.close_button.clicked.connect(dialog.reject)
            self.open_button.clicked.connect(self.done)
            self.tree_widget.itemDoubleClicked.connect(self.done)
        else:
            raise Exception

    def done(self, _=None):
        self.dialog.done(0)

    def populate(self, examples_dict):
        self.examples_dict = examples_dict
        self.tree_widget.clear()

        for path, examples in examples_dict.items():
            for ex in examples:
                rel_path = os.path.relpath(os.path.dirname(ex['path']), path)
                split_rel_path = os.path.normpath(rel_path).split(os.path.sep)
                parent_path = "/".join(split_rel_path[0:-1])
                if rel_path not in self.dir_items:
                    dir_ = None
                    if parent_path:
                        dir_ = QTreeWidgetItem(self.dir_items[parent_path])
                    else:
                        dir_ = QTreeWidgetItem(self.tree_widget)
                    dir_.setText(0, split_rel_path[-1])
                    self.dir_items[rel_path] = dir_
                item = QTreeWidgetItem(self.dir_items[rel_path])
                item.setText(0, ex["title"] if ex["title"] else ex["name"])
                item.setData(0, self.data_role, QVariant(ex))

        self.tree_widget.setSortingEnabled(True)
        self.tree_widget.sortByColumn(0, Qt.AscendingOrder)

    def reset_preview(self):
        self.title_label.setText(f"<b>Title:</b> ")
        self.author_label.setText(f"<b>Author:</b> ")
        self.language_label.setText(f"<b>Output language:</b> ")
        self.gen_opts_label.setText(f"<b>Type:</b> ")
        self.desc_label.setText('')
        self.image_label.setPixmap(QPixmap())

    def populate_preview(self):
        ex = self.tree_widget.currentItem().data(0, self.data_role)

        self.title_label.setText(f"<b>Title:</b> {ex['title'] if ex else ''}")
        self.author_label.setText(f"<b>Author:</b> {ex['author'] if ex else ''}")
        try:
            self.language_label.setText(f"<b>Output language:</b> {self.lang_dict[ex['output_language']] if ex else ''}")
            self.gen_opts_label.setText(f"<b>Type:</b> {self.gen_opts_dict[ex['generate_options']] if ex else ''}")
        except KeyError:
            self.language_label.setText(f"<b>Output language:</b> ")
            self.gen_opts_label.setText(f"<b>Type:</b> ")
        self.desc_label.setText(ex["desc"] if ex else '')

        if ex:
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
        else:
            self.image_label.setPixmap(QPixmap())


    def open_file(self):
        ex = self.tree_widget.currentItem().data(0, self.data_role)
        self.file_to_open.emit(ex["path"])

    def filter_(self, key: str):
        """
        Only display examples that contain a specific block. (Hide the others)

            Parameters:
                key: The key of the block to search for
        """
        found = False
        ex_paths = self.library.get_examples(key)
        for i in range(self.tree_widget.topLevelItemCount()):
            top = self.tree_widget.topLevelItem(i)
            if self.show_selective(top, ex_paths):
                found = True
        return found


    def show_selective(self, item, path):
        item.setHidden(True)
        if item.childCount():  # is a directory
            for i in range(item.childCount()):
                if self.show_selective(item.child(i), path):
                    item.setHidden(False)
            return not item.isHidden()
        else:  # is an example
            ex = item.data(0, self.data_role)
            if ex['path'] in path:
                item.setHidden(False)
                return True
            else:
                return False

    def show_all(self, item):
        item.setHidden(False)
        for i in range(item.childCount()):
            self.show_all(item.child(i))

    def reset(self):
        """Reset the filter, collapse all."""
        self.tree_widget.collapseAll()
        self.reset_preview()

        for i in range(self.tree_widget.topLevelItemCount()):
            top = self.tree_widget.topLevelItem(i)
            self.show_all(top)


    def find_examples(self, progress_callback, ext="grc"):
        """Iterate through the example flowgraph directories and parse them."""
        examples_dict = {}
        with Cache(Constants.EXAMPLE_CACHE_FILE, log=False) as cache:
            for entry in self.platform.config.example_paths:
                if entry == '':
                    log.error("Empty example path!")
                    break
                examples_dict[entry] = []
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
                                example["title"] = data["options"]["parameters"]["title"] or ""
                                example["desc"] = data["options"]["parameters"]["description"] or ""
                                example["author"] = data["options"]["parameters"]["author"] or ""
                                example["path"] = file_path
                                example["module"] = os.path.dirname(file_path).replace(entry, "")
                                if example["module"].startswith("/"):
                                    example["module"] = example["module"][1:]

                                example["blocks"] = set()
                                for block in data["blocks"]:
                                    example["blocks"].add(block["id"])
                                examples_dict[entry].append(example)
                            except Exception:
                                continue

        examples_w_block: dict[str, set[str]] = {}
        designated_examples_w_block: dict[str, set[str]] = {}
        for path, examples in examples_dict.items():
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

        return (examples_dict, examples_w_block, designated_examples_w_block)
