# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os

from .exceptions import UserFileException, UserParseException
from .module import resolve_abi_version
from .toml import toml_load


def get_bindings_configuration(abi_major, sip_file, sip_include_dirs):
    """ Get the configuration of a set of bindings. """

    # We make no assumption about the name of the .sip file but we assume that
    # the directory it is in is the name of the bindings.
    bindings_name = os.path.basename(os.path.dirname(sip_file))

    # See if there is a .toml file.
    for sip_dir in sip_include_dirs:
        toml_file = os.path.join(sip_dir, bindings_name,
                bindings_name + '.toml')

        if os.path.isfile(toml_file):
            break
    else:
        return [], []

    # Read the configuration.
    try:
        cfg = toml_load(toml_file)
    except Exception as e:
        raise UserParseException(toml_file, detail=str(e))

    # Check the ABI version is compatible.
    cfg_abi_version = cfg.get('sip-abi-version')
    if cfg_abi_version is None or not isinstance(cfg_abi_version, str):
        raise UserFileException(toml_file,
                "'sip-abi-version' must be specified as a string")

    cfg_abi_major = int(resolve_abi_version(cfg_abi_version).split('.')[0])

    if cfg_abi_major != abi_major:
        raise UserFileException(toml_file,
                "'{0}' was built against ABI v{1} but this module is being "
                        "built against ABI v{2}".format(bindings_name,
                                cfg_abi_major, abi_major))

    # Return the tags and disabled features.
    return (_get_string_list(toml_file, cfg, 'module-tags'),
            _get_string_list(toml_file, cfg, 'module-disabled-features'))


def _get_string_list(toml_file, cfg, name):
    """ Get an option from the configuration and check it is a list of strings.
    """

    option_list = cfg.get(name)
    if option_list is None:
        option_list = list()
    elif not isinstance(option_list, list):
        raise UserFileException(toml_file, "'{0}' must be a list".format(name))

    for option in option_list:
        if not isinstance(option, str):
            raise UserFileException(toml_file,
                    "elements of '{0}' must be strings".format(name))

    return option_list
