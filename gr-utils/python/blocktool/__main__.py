#
# Copyright 2019 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
# 
#
""" main function to run the blocktool api from the command line. """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import sys
from .cli import cli


sys.exit(cli())
