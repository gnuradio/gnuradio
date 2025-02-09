# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ._version import version, version_tuple


# Convert the setuptools_scm generated version number to our historic names and
# formats.

SIP_VERSION_STR = version
SIP_VERSION = (version_tuple[0] << 16) + (version_tuple[1] << 8) + version_tuple[2]
