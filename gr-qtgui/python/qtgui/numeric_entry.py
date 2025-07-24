#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2024 Philipp Niedermayer.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from PyQt5 import Qt, QtCore, QtWidgets
from gnuradio import gr
import math
import re
import ast


def numeric_eval(expression):
    """Securely evaluates the numeric expression with simple arithmetics"""
    save_operations = {
        ast.UAdd: lambda a: a,
        ast.USub: lambda a: -a,
        ast.Add: lambda a, b: a + b,
        ast.Sub: lambda a, b: a - b,
        ast.Mult: lambda a, b: a * b,
        ast.Div: lambda a, b: a / b,
        ast.Pow: lambda a, b: a**b,
    }

    def evaluate(node):
        if isinstance(node, ast.Expr):
            return evaluate(node.value)
        if isinstance(node, ast.Constant):
            return node.value
        if isinstance(node, ast.UnaryOp):
            if op := save_operations.get(node.op.__class__, None):
                return op(evaluate(node.operand))
        if isinstance(node, ast.BinOp):
            if op := save_operations.get(node.op.__class__, None):
                return op(evaluate(node.left), evaluate(node.right))
        raise TypeError("Unsupported operation: %s" % node.__class__.__name__)

    return float(evaluate(ast.parse(expression).body[-1]))


class NumericEntry(Qt.QToolBar):
    def __init__(self, callback, label="", value=0, increment=0, unit="", description=None, precision=10, enabled=True):
        QtWidgets.QWidget.__init__(self)

        self.value = value
        self.value_min = self.value_max = None
        self.increment = increment
        self.unit = unit
        self.description = description
        self.precision = precision
        self.callback = callback

        self.label = Qt.QLabel(label + ": ")
        self.addWidget(self.label)

        self.edit = Qt.QLineEdit()
        self.edit.editingFinished.connect(self._apply)
        self.edit.textEdited.connect(lambda: self._set_editing())
        self._editing = (False, None)
        self.addWidget(self.edit)

        self._update_tooltip()
        self.set_value(value)
        self.set_enabled(enabled)

    def set_enabled(self, enabled):
        self.edit.setEnabled(enabled)

    def keyPressEvent(self, event):
        # in-/decrement
        if event.key() == QtCore.Qt.Key_Up and self.increment:
            self._apply(self.value + self.increment)
        elif event.key() == QtCore.Qt.Key_Down and self.increment:
            self._apply(self.value - self.increment)
        elif event.key() == QtCore.Qt.Key_PageUp and self.increment:
            self._apply(self.value + 10 * self.increment)
        elif event.key() == QtCore.Qt.Key_PageDown and self.increment:
            self._apply(self.value - 10 * self.increment)
        else:
            super().keyPressEvent(event)

    def _set_editing(self, editing=True, valid=True):
        if self._editing != (editing, valid):
            style = "QLabel { color: red }" if not valid else \
                    "QLabel { color: blue }" if editing else ""
            Qt.QMetaObject.invokeMethod(self.label, "setStyleSheet", Qt.Q_ARG("QString", style))
            self._editing = (editing, valid)

    def _fmt(self, value):
        return f"{value:.{self.precision}g} {self.unit}".strip()

    def _update_tooltip(self):
        tip = ""
        if self.description:
            tip += f"{self.description}\n"
        if self.value_min is not None and not math.isinf(self.value_min):
            tip += f"Minimum value: {self._fmt(self.value_min)}\n"
        if self.value_max is not None and not math.isinf(self.value_max):
            tip += f"Maximum value: {self._fmt(self.value_max)}\n"
        if self.increment:
            tip += f"Increment (up/down key): {self._fmt(self.increment)}\n"
        self.label.setToolTip(tip.strip())

    def set_limits(self, lower, upper):
        self.value_min = lower
        self.value_max = upper
        self._update_tooltip()
        self._apply()

    def set_value(self, value):
        """Public method used to set new value"""
        self.value = float(value)
        Qt.QMetaObject.invokeMethod(self.edit, "setText", Qt.Q_ARG("QString", self._fmt(self.value)))
        self._set_editing(False)

    def _parse(self, string):
        SI = dict(G=1e9, M=1e6, k=1e3, m=1e-3, u=1e-6, μ=1e-6, n=1e-9)
        scale = 1
        # handle physical units
        if self.unit:
            if string.endswith(self.unit):
                string = string[:-len(self.unit)].strip()
            elif len(self.unit) > 1:
                for prefix, factor in SI.items():
                    if self.unit.startswith(prefix) and string.endswith(self.unit[1:]):
                        string = string[:-len(self.unit[1:])].strip()
                        scale /= factor
        # handle engeneering shortforms like "1k"
        if string[-1] in SI:
            scale *= SI[string[-1]]
            string = string[:-1]
        # handle common typos
        string = string.replace(',', '.').strip()
        # evaluate
        try:
            value = float(string)
        except ValueError:
            # check string before evaluation for security
            string = string.replace(self.unit, '')
            if match := re.search(r'[^0-9.e+\-*/() ]', string):
                raise ValueError("Characters not allowed: " + match.group())
            value = float(numeric_eval(string))

        return scale * value

    def _apply(self, value=None):
        if value is None:
            value = self.edit.text()

        if isinstance(value, str):
            # parse string
            value = value.strip()
            try:
                if value == "":
                    value = self.value
                else:
                    value = self._parse(value)
            except Exception as e:
                self._set_editing(valid=False)
                return

        # apply limits
        if self.value_min is not None:
            value = max(self.value_min, value)
        if self.value_max is not None:
            value = min(self.value_max, value)

        # apply precision
        value = float(f"{value:.{self.precision}g}")

        # call callback if it changed
        if value != self.value:
            self.value = value
            self.callback(value)

        # set text to reflect actual value
        self.set_value(self.value)
