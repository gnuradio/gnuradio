# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

from ._lazy import lazy_property, nop_write

from .evaluated import (
    Evaluated,
    EvaluatedEnum,
    EvaluatedPInt,
    EvaluatedFlag,
    setup_names,
)
