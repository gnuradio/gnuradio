"""
Copyright 2021 Marcus Müller
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later
"""

from enum import Flag, auto
from functools import cache
from typing import Union, Iterable
from ..core.base import Element
from .canvas.colors import get_color

from .defaulttheme import DEFAULT_THEME


class State(Flag):
    """
    State to query for
    """
    DEFAULT = auto()
    DISABLED = auto()
    SELECTED = auto()
    BYPASSED = auto()
    DEPRECATED = auto()
    INVALID_PARAMETERS = auto()
    INVALID_CONNECTION = auto()
    MISSING = auto()
    UNSPECIFIED_ERROR = auto()
    PROBLEM = (DEPRECATED | INVALID_PARAMETERS | INVALID_CONNECTION
               | UNSPECIFIED_ERROR | MISSING)

    @staticmethod
    def reduce(iterable: Iterable):
        """
        reduce an iterable of states to the combined state
        """
        if not iterable:
            return State.DEFAULT
        sumstate = iterable[0]
        for state in iterable:
            sumstate = sumstate | state
        return sumstate


class ThemeEngine:
    """
    Theming Engine.

    Able to load themes, categorize them hierarchically:

    - Object Type 1:
      - Default:
        - Property1: ValueA
        - Property2: ValueB
      - Single state selector:
        - Property1: ValueC
        - Property2: ValueD
      - Space-joined combination of state selectors:
        - Property1: ValueE
        - Property3: ValueF

    Note that the more specific a state selector is, i.e. the more elements
    in the selector list, the higher its priority.

    To give an example how that would be encoded in JSON:
    ```
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
    ```
    """
    def __init__(self):
        self.dict = {}

    def load(self, dictlike):
        """
        updates the internal dict with new values from dictlike
        """
        for typename, typedict in dictlike.items():
            if typename not in self.dict:
                self.dict[typename] = {}
            for statestring, stateprops in typedict.items():
                states = statestring.split(" ")
                statecomb = []
                for state in states:
                    if not state.upper() in State.__members__.keys():
                        # raise Exception(f"{typename}: Unknown state {state}")
                        print(f"Unknown state {state}, ignoring")
                        print("possible states: " +
                              ", ".join(State.__members__.keys()))
                        continue
                    statecomb.append(State.__members__[state.upper()])
                if not statecomb:
                    print(f"{typename}: statestring \"{statestring}\"" +
                          " contained no states")
                    continue
                sumstate = State.reduce(statecomb)
                self.dict[typename][sumstate] = stateprops

        self.get_property.cache_clear()
        self.get_type_specific_dict.cache_clear()

    @cache
    def get_type_specific_dict(self, objtype: Union[Element, type, str]):
        """
        Retrieve the type-specific dict

        This got refactored out to improve caching
        """
        type_specific = None
        if isinstance(objtype, str):
            try:
                type_specific = self.dict[objtype]
            except KeyError:
                return None
        if isinstance(objtype, Element):
            objtype = type(objtype)
        if isinstance(objtype, type):
            for type_candidate in objtype.mro():
                try:
                    classname = type_candidate.__name__
                    if classname == "object":
                        break
                    type_specific = self.dict[classname]
                    break
                except KeyError:
                    continue
        return type_specific

    @cache
    def get_property(self,
                     property: str,
                     objtype: Union[Element, type, str],
                     state: State = State.DEFAULT,
                     default=None):
        """
        Get the property
        """

        type_specific = self.get_type_specific_dict(objtype)
        if not type_specific:
            return default

        # select the most specific specification
        # Due to our construction of binary flags,
        # the state with the most 1 set is the most specific
        max_bit_count, to_apply = -1, default
        for statecomb, props in type_specific.items():
            if statecomb == State.DEFAULT or statecomb in state:
                if property in props:
                    count_of_ones = bin(state.value).count("1")
                    if count_of_ones >= max_bit_count:
                        to_apply = props[property]

        if isinstance(to_apply, str) and "color" in property:
            return get_color(to_apply)
        return to_apply


Theme = ThemeEngine()
Theme.load(DEFAULT_THEME)
