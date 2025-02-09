# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os

from ..exceptions import UserException


# The directory containing the different module implementations.
_module_source_dir = os.path.join(os.path.dirname(__file__), 'source')


def get_module_source_dir(abi_major_version):
    """ Return the name of the directory containing the latest source of the
    sip module that implements the given ABI version.
    """

    return os.path.join(_module_source_dir, abi_major_version)


def get_sip_module_version(abi_major_version):
    """ Return the version number of the latest implementation of the sip
    module with the given ABI as a string.
    """

    abi_minor_version = patch_version = None

    # Read the version from the header file shared with the code generator.
    with open(os.path.join(get_module_source_dir(abi_major_version), 'sip.h.in')) as vf:
        for line in vf:
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == '#define':
                name = parts[1]
                value = parts[2]

                if name == 'SIP_ABI_MINOR_VERSION':
                    abi_minor_version = value
                elif name == 'SIP_MODULE_PATCH_VERSION':
                    patch_version = value

    # These are internal errors and should never happen.
    if abi_minor_version is None:
        raise ValueError(
                f"'SIP_ABI_MINOR_VERSION' not found for ABI {abi_major_version}")

    if patch_version is None:
        raise ValueError(
                f"'SIP_MODULE_PATCH_VERSION' not found for ABI {abi_major_version}")

    return f'{abi_major_version}.{abi_minor_version}.{patch_version}'


def resolve_abi_version(abi_version, module=True):
    """ Return a valid ABI version or the latest if none was given. """

    # Get the major and minimum minor version of what we need.
    if abi_version:
        parts = abi_version.split('.')
        abi_major_version = parts[0]

        if not os.path.isdir(get_module_source_dir(abi_major_version)):
            raise UserException(
                    f"'{abi_version}' is not a supported ABI version")

        if len(parts) == 1:
            minimum_minor_version = 0
        else:
            try:
                minimum_minor_version = int(parts[1])
            except ValueError:
                minimum_minor_version = None

            if len(parts) > 2 or minimum_minor_version is None:
                raise UserException(
                        f"'{abi_version}' is not a valid ABI version")
    else:
        abi_major_version = sorted(os.listdir(_module_source_dir), key=int)[-1]
        # v13.0 is deprecated so explicitly exclude it to avoid a later
        # deprecation warning.
        minimum_minor_version = 1 if abi_major_version == '13' else 0

    # Get the minor version of what we actually have.
    module_version = get_sip_module_version(abi_major_version)
    _, abi_minor_version, _ = module_version.split('.')

    # Check we meet the minimum requirement.
    if int(abi_minor_version) < minimum_minor_version:
        raise UserException(f"'{abi_version}' is not a supported ABI version")

    if module:
        # Return the module's version.
        return f'{abi_major_version}.{abi_minor_version}'

    # Return the required version.
    return f'{abi_major_version}.{minimum_minor_version}'
