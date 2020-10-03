# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from .utils import Message, Spec


class Validator(object):

    def __init__(self, scheme=None):
        self._path = []
        self.scheme = scheme
        self.messages = []
        self.passed = False

    def run(self, data):
        if not self.scheme:
            return True
        self._reset()
        self._path.append('block')
        self._check(data, self.scheme)
        self._path.pop()
        return self.passed

    def _reset(self):
        del self.messages[:]
        del self._path[:]
        self.passed = True

    def _check(self, data, scheme):
        if not data or not isinstance(data, dict):
            self._error('Empty data or not a dict')
            return
        if isinstance(scheme, dict):
            self._check_dict(data, scheme)
        else:
            self._check_var_key_dict(data, *scheme)

    def _check_var_key_dict(self, data, key_type, value_scheme):
        for key, value in data.items():
            if not isinstance(key, key_type):
                self._error('Key type {!r} for {!r} not in valid types'.format(
                    type(value).__name__, key))
            if isinstance(value_scheme, Spec):
                self._check_dict(value, value_scheme)
            elif not isinstance(value, value_scheme):
                self._error('Value type {!r} for {!r} not in valid types'.format(
                    type(value).__name__, key))

    def _check_dict(self, data, scheme):
        for key, (types_, required, item_scheme) in scheme.items():
            try:
                value = data[key]
            except KeyError:
                if required:
                    self._error('Missing required entry {!r}'.format(key))
                continue

            self._check_value(value, types_, item_scheme, label=key)

        for key in set(data).difference(scheme):
            self._warn('Ignoring extra key {!r}'.format(key))

    def _check_list(self, data, scheme, label):
        for i, item in enumerate(data):
            self._path.append('{}[{}]'.format(label, i))
            self._check(item, scheme)
            self._path.pop()

    def _check_value(self, value, types_, item_scheme, label):
        if not isinstance(value, types_):
            self._error('Value type {!r} for {!r} not in valid types'.format(
                type(value).__name__, label))
        if item_scheme:
            if isinstance(value, list):
                self._check_list(value, item_scheme, label)
            elif isinstance(value, dict):
                self._check(value, item_scheme)

    def _error(self, msg):
        self.messages.append(Message('.'.join(self._path), 'error', msg))
        self.passed = False

    def _warn(self, msg):
        self.messages.append(Message('.'.join(self._path), 'warn', msg))
