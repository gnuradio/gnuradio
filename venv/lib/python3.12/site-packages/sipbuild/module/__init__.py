# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


# Publish the API.  This is private to the rest of sip.
from .abi_version import resolve_abi_version
from .module import copy_nonshared_sources, copy_sip_h, copy_sip_pyi, module
