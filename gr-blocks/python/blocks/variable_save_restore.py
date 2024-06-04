#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 Philipp Niedermayer.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr
import yaml
import pmt
import os


class SafeDumper(yaml.SafeDumper):
    """A safe yaml dumper with support for additional datatypes"""
    pass


class SafeLoader(yaml.SafeLoader):
    """A safe yaml loader with support for additional datatypes"""
    pass


SafeDumper.add_representer(complex, yaml.representer.Representer.represent_complex)
SafeDumper.add_representer(tuple, yaml.representer.Representer.represent_tuple)
SafeLoader.add_constructor(u"tag:yaml.org,2002:python/complex", yaml.constructor.Constructor.construct_python_complex)
SafeLoader.add_constructor(u"tag:yaml.org,2002:python/tuple", yaml.constructor.Constructor.construct_python_tuple)


class variable_save_restore(gr.sync_block):
    """
    (see description in this blocks yml file)
    """

    def __init__(self, top_block=None, slot="default", variables="", save_trigger=False, restore_trigger=False, description_variable=None):
        gr.sync_block.__init__(self, name="variable_save_restore", in_sig=[], out_sig=None)
        self.message_port_register_in(pmt.intern("save"))
        self.set_msg_handler(pmt.intern("save"), self.handle_msg_save)
        self.message_port_register_in(pmt.intern("restore"))
        self.set_msg_handler(pmt.intern("restore"), self.handle_msg_restore)

        self.filename = os.path.join(gr.paths.userconf(), "saverestore", f"{top_block.__class__.__name__}.yml")
        self.filename = os.path.expanduser(self.filename)
        self.logger.info(f"Config file: {self.filename}")
        os.makedirs(os.path.dirname(self.filename), exist_ok=True)
        self.load_storage()

        self.top_block = top_block
        self.description_variable = description_variable
        self.set_slot(slot)
        self.set_variables(variables)
        self.save_trigger = save_trigger
        self.restore_trigger = restore_trigger

    def set_slot(self, slot):
        self.slot = slot
        if self.description_variable:
            data = self.storage.get(self.slot, {})
            info = data.get(self.description_variable, "")
            self.set_variable_values({self.description_variable: info})

    def set_variables(self, variables):
        if isinstance(variables, str):
            variables = [v.strip() for v in variables.split(",") if v.strip()]
        self.variables = variables
        if self.description_variable:
            self.variables.append(self.description_variable)

    def set_save_trigger(self, save_trigger):
        if save_trigger and not self.save_trigger:
            self.save()
        self.save_trigger = save_trigger

    def set_restore_trigger(self, restore_trigger):
        if restore_trigger and not self.restore_trigger:
            self.restore()
        self.restore_trigger = restore_trigger

    def handle_msg_save(self, msg):
        self.save()

    def handle_msg_restore(self, msg):
        self.restore()

    def get_variable_values(self):
        """Get values of variables using top-glock getters"""
        data = {}
        for name in self.variables:
            try:
                data[name] = self._get_variable(self.top_block, name)
            except AttributeError as e:
                self.logger.error(
                    f"AttributeError:{e}\nERROR: Failed to get variable `{name}`"
                )

        return data

    def _get_variable(self, block, name):
        if "." in name:
            subblock, name = name.split(".", maxsplit=1)
            return self._get_variable(getattr(block, subblock), name)
        return getattr(block, f"get_{name}")()

    def set_variable_values(self, data, verbose=False):
        """Set values of variables using top-glock setters"""
        n = 0
        for name, val in data.items():
            if verbose:
                self.logger.debug(f"{name:20s}    {val}")
            try:
                self._set_variable(self.top_block, name, val)
                n += 1
            except AttributeError as e:
                self.logger.error(
                    f"AttributeError:{e}\nERROR: Failed to set variable `{name}` to `{val}`"
                )
        return n

    def _set_variable(self, block, name, val):
        if "." in name:
            subblock, name = name.split(".", maxsplit=1)
            return self._set_variable(getattr(block, subblock), name, val)
        setter = getattr(block, f"set_{name}")
        setter(val)

    def save(self):
        data = self.get_variable_values()
        self.storage[self.slot] = data
        try:
            self.save_storage()
        except Exception as e:
            self.logger.error(
                f"Exception:{e}\nFailed to save variables to slot {self.slot}"
            )
        else:
            self.logger.info(
                f"Saved {len(data)} of {len(self.variables)} variables to slot {self.slot}"
            )

    def restore(self):
        data = self.storage.get(self.slot, {})
        data = {k: v for k, v in data.items() if k in self.variables}
        n = self.set_variable_values(data)
        self.logger.info(
            f"Restored {n} of {len(self.variables)} variables from slot {self.slot}"
        )

    def load_storage(self):
        """Load from disk into memory"""
        if os.path.exists(self.filename):
            with open(self.filename, 'r') as handle:
                self.storage = yaml.load(handle, SafeLoader)
        else:
            self.storage = {}

    def save_storage(self):
        """Save from memory to disk"""
        with open(self.filename, 'w') as handle:
            yaml.dump(self.storage, handle, SafeDumper)
