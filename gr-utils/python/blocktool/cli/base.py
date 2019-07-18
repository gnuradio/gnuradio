#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#
""" Base class for blocktool cli """

import sys
import click
from click import ClickException


class BlockToolException(ClickException):
    """ Exception class for enhanced CLI interface """

    def show(self, file=None):
        """ displays the colored message """
        click.secho('BlockToolException: {}'.format(
            self.format_message()), fg='red')


def run(module):
    """Call the run function of the core modules."""
    try:
        module.run()
    except BlockToolException as err:
        click.echo(err, file=sys.stderr)
        exit(1)
