import os

from os import path

import pytest

import sys
import inspect

from grc.core.platform import Platform

# Gather blocks
BLOCK_PATHS = []
ROOT = path.join(path.dirname(__file__), '../..')
BLOCK_PATHS = [path.join(path.dirname(__file__), '../../grc/blocks'), '../../build/gr-uhd/grc']

EVALUATED = False
EVALUATED_PATH = None

def audit(event, args):
    global EVALUATED, EVALUATED_PATH

    if event == 'exec':
        stack = inspect.stack()
        path = stack[1].filename

        if "/grc/" in path:
            EVALUATED = True
            EVALUATED_PATH = path

def open_flowgraph(path):
    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library(BLOCK_PATHS)

    flow_graph = platform.make_flow_graph(path)
    flow_graph.rewrite()
    flow_graph.validate()

@pytest.mark.skipif(sys.version_info < (3,8), reason="requires python 3.8")
def test_eval():
    global EVALUATED, EVALUATED_PATH

    sys.addaudithook(audit)

    flowgraph_path = path.join(path.dirname(__file__), 'resources', 'test_eval.grc')
    open_flowgraph(flowgraph_path)

    assert not EVALUATED, f"untrusted evaluation in {EVALUATED_PATH}"
