#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Returns information about a module """


import click

from ..core import ModToolInfo
from .base import common_params, run


@click.command('info')
@click.option('--python-readable', is_flag=True,
              help="Return the output in a format that's easier to read for Python scripts.")
@click.option('--suggested-dirs',
              help="Suggest typical include dirs if nothing better can be detected.")
@common_params
def cli(**kwargs):
    """ Return information about a given module """
    self = ModToolInfo(**kwargs)
    run(self)
