#
# Copyright 2024 MindSlayer001.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""Returns information about a module"""

import subprocess
import click


@click.command(name="help")
def cli():
    """
    Display help for gr_modtool (equivalent to `gr_modtool --help`).
    """
    ctx = click.get_current_context()
    click.echo(ctx.parent.get_help())
