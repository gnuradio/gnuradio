"""
Copyright 2021 Marcus Müller
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later
"""

DEFAULT_THEME = {
    "Element": {
        "default": {
            "font-family": "Sans",
            "font-size": 8,
        },
    },
    "Flowgraph": {
        "default": {
            "background-color": "#FFFFFF"
        }
    },
    "Block": {
        "default": {
            "border-color": "#616161",
            "color": "#000000",
            "background-color": "#F1ECFF",
            "font-family": "Sans",
            "font-size": 8,
            "padding": 7,
        },
        "disabled": {
            "background-color": "#CCCCCC",
            "border-color": "#888888",
            "color": "#333333"
        },
        "bypassed": {
            "background-color": "#F4FF81"
        },
        "selected": {
            "border-color": "#00FFFF"
        },
        "missing": {
            "background-color": "#F4FF81"
        }
    },
    "Connection": {
        "default": {
            "color": "#000000"
        },
        "disabled": {
            "color": "#BBBBBB"
        },
        "invalid_connection": {
            "color": "#FF0000"
        }
    },
    "Port": {
        "default": {
            "background-color": "#FFFFFF",
            "font-size": 8,
            "padding": 2
        }
    },
    "Param": {
        "default": {
            "font-size": 7.5,
        }
    },
    "Comment": {
        "default": {
            "color": "#404040",
            "font-size": 8,
            "font-family": "Sans",
        },
        "disabled": {
            "color": "#808080"
        }
    }
}
