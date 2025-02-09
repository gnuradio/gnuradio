# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from argparse import ArgumentParser as ArgParser

from ._version import version


class ArgumentParser(ArgParser):
    """ An argument parser for all sip command line tools. """

    def __init__(self, description, build_tool=False, **kwargs):
        """ Initialise the parser. """

        super().__init__(description=description, **kwargs)

        self.add_argument('-V', '--version', action='version', version=version)

        # This option is handled by the bootstrap process and is only here to
        # contribute to the help.
        if build_tool:
            self.add_argument('--deprecations-are-errors', action='store_true',
                    help="using deprecated features is an error")
