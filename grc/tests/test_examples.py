import os

from os import path

import pytest

from grc.core.platform import Platform

try:
    os.mkdir(path.join(path.dirname(__file__), 'resources/tests'))
except FileExistsError:
    pass

# Gather blocks
BLOCK_PATHS = []
ROOT = path.join(path.dirname(__file__), '../..')
BLOCK_PATHS = [path.join(path.dirname(__file__), '../../grc/blocks'), '../../build/gr-uhd/grc']
for file_dir in os.scandir(ROOT):
    # If it is a module
    if path.isdir(file_dir) and file_dir.name.startswith("gr-"):
        BLOCK_PATHS.append(path.join(file_dir, "grc"))

def gather_examples():
    global ROOT
    example_paths = []
    for file_dir in os.scandir(ROOT):
        # If it is a module
        if path.isdir(file_dir) and file_dir.name.startswith("gr-"):
            try:
                for pos_ex in os.scandir(path.join(file_dir, "examples")):
                    if path.isfile(pos_ex) and pos_ex.name.endswith(".grc"):
                        example_paths.append(pos_ex)

                # Some modules have their .grc files in a subdirectory called "grc"
                for pos_ex in os.scandir(path.join(file_dir, "examples/grc")):
                    if path.isfile(pos_ex) and pos_ex.name.endswith(".grc"):
                        example_paths.append(pos_ex)
            except FileNotFoundError:
                continue
    return example_paths

def print_proper(element):
    if element.is_block:
        return element.name
    return f"{element.parent.name} - {element}"

@pytest.mark.examples
@pytest.mark.parametrize("example", gather_examples())
def test_all_examples(example):
    global BLOCK_PATHS

    print(example.path)

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library(BLOCK_PATHS)

    flow_graph = platform.make_flow_graph(example.path)
    flow_graph.rewrite()
    flow_graph.validate()

    assert flow_graph.is_valid(), (example.name, [f"{print_proper(elem)}: {msg}" for elem, msg in flow_graph.iter_error_messages()])

    generator = platform.Generator(flow_graph, path.join(path.dirname(__file__), 'resources/tests'))
    generator.write()
