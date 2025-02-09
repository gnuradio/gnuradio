# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


try:
    import tomllib
except ImportError:
    import tomli as tomllib


def toml_load(toml_file):
    """ Return a dict containing the decoded contents of a TOML file. """

    with open(toml_file, 'rb') as f:
        return tomllib.load(f)


def toml_loads(toml_str):
    """ Return a dict containing the decoded contents of a TOML string. """

    return tomllib.loads(toml_str)
