# Copyright 2021 Marcus Müller
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

from grc.gui.Theme import ThemeEngine, State
from grc.gui.canvas.colors import get_color
from grc.core.blocks import Block
import json


def test_loading(engine: ThemeEngine = None):
    """
    Test whether a simple JSON string can be loaded
    """
    jsonstring = """
    {
    "Block": {
        "default": {
            "border-color": "#000000",
            "background-color": "#DEDEDE",
            "border-width": "3"
        },
        "selected": {
            "border-color": "#0000FF"
        },
        "disabled": {
            "background-color": "#CCCCCC",
            "border-color": "#444444"
        },
        "selected disabled": {
            "border-color": "#4444DE"
        }
        }
    }
    """
    dic = json.loads(jsonstring)
    if not engine:
        engine = ThemeEngine()
    engine.load(dic)


def test_getting():
    """
    Test whether properties can be retrieved in general
    """
    engine = ThemeEngine()
    test_loading(engine)

    prop = engine.get_property("border-color", Block)
    assert prop == get_color("#000000"), f"default property incorrect: {prop}"

    prop = engine.get_property("border-color",
                               Block,
                               state=State.INVALID_CONNECTION)
    assert prop == get_color("#000000"),\
        f"default fallthrough property incorrect: {prop}"

    prop = engine.get_property("border-color", Block, State.SELECTED)
    assert prop == get_color("#0000FF"),\
        f"selected property incorrect: {prop}"

    prop = engine.get_property("border-color", Block,
                               State.SELECTED | State.DISABLED)
    assert prop == get_color("#4444DE"),\
        f"combined selected property incorrect: {prop}"
