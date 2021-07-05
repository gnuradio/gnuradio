import copy
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
PLATFORM = None

try:
    os.mkdir(path.join(path.dirname(__file__), 'resources/temp'))
except FileExistsError:
    pass

def audit(event, args):
    global EVALUATED, EVALUATED_PATH

    if event == 'exec':
        stack = inspect.stack()
        path = stack[1].filename

        if "/grc/" in path:
            EVALUATED = True
            EVALUATED_PATH = path

def intree_platform():
    platform = Platform(
        name="GNU Radio Companion Compiler",
        prefs=None,
        version="0.0.0",
    )
    platform.build_library([str(p) for p in BLOCK_PATHS])

    return platform

def gather_data():
    global PLATFORM
    if not PLATFORM:
        PLATFORM = intree_platform()
    data = []
    
    # parse file
    flowgraph_path = path.join(path.dirname(__file__), 'resources', 'test_eval.grc')
    fg_data = PLATFORM.parse_flow_graph(flowgraph_path)

    # missing no data
    data.append((fg_data, "MISSING NOTHING"))

    # missing one param
    missing_one_param = copy.deepcopy(fg_data)
    for block in missing_one_param['blocks']:
        if block['name'] == 'blocks_selector_0':
            del block['evaluated']['num_inputs']
    data.append((missing_one_param, "MISSING ONE PARAM"))

    # missing all params
    missing_all_params = copy.deepcopy(fg_data)
    for block in missing_all_params['blocks']:
        del block['evaluated']
    data.append((missing_all_params, "MISSING ALL PARAMS"))

    # missing one expression
    missing_one_expr = copy.deepcopy(fg_data)
    for block in missing_one_expr['blocks']:
        if block['name'] == 'blocks_selector_0':
            del block['expressions']['num_inputs']
    data.append((missing_one_expr, "MISSING ONE EXPRESSION"))

    # missing all expressions
    missing_all_exprs = copy.deepcopy(fg_data)
    for block in missing_all_exprs['blocks']:
        del block['expressions']
    data.append((missing_all_exprs, "MISSING ALL EXPRESSIONS"))

    # missing all params and expressions
    missing_all_saved = copy.deepcopy(fg_data)
    for block in missing_all_saved['blocks']:
        del block['evaluated']
        del block['expressions']
    data.append((missing_all_saved, "MISSING ALL SAVED"))

    return data

@pytest.mark.skipif(sys.version_info < (3,8), reason="requires python 3.8")
@pytest.mark.parametrize("data", gather_data())
def test_eval(data):
    global EVALUATED, EVALUATED_PATH
    EVALUATED = False
    EVALUATED_PATH = None

    sys.addaudithook(audit)

    tmp_file = path.join(path.dirname(__file__), 'resources', 'temp', 'test_eval.grc')
    PLATFORM.save_flow_graph_data(tmp_file, data=data[0])

    fg = PLATFORM.make_flow_graph(from_filename=tmp_file)
    fg.rewrite()
    fg.validate()

    assert not EVALUATED, f"untrusted evaluation in {EVALUATED_PATH}, with data {data[1]}"
