#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Returns information about a module """


import subprocess
import click

@click.command(name="help")
def cli():
    """
    Display help for gr_modtool (equivalent to `gr_modtool --help`).
    """
    try:
        result = subprocess.run(
            ["gr_modtool", "--help"],
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        click.echo(result.stdout)
    except subprocess.CalledProcessError as e:
        click.echo("Error occurred while running gr_modtool --help:", err=True)
        click.echo(e.stderr, err=True)

