# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os
import sys

from .version import SIP_VERSION


# Set if deprecations should be treated as errors.
_deprecations_are_errors = False


def set_deprecations_are_errors(deprecations_are_errors):
    """ If set then deprecations will be handled as errors. """

    global _deprecations_are_errors
    _deprecations_are_errors = deprecations_are_errors


class UserException(Exception):
    """ An exception capturing user friendly information. """

    def __init__(self, text, *, detail=None):
        """ Initialise the exception with its user friendly text and the
        optional detail.
        """

        super().__init__()

        self.text = text
        self.detail = detail


class UserFileException(UserException):
    """ An exception related to a specific file. """

    def __init__(self, filename, text, *, detail=None):
        """ Initialise the exception. """

        super().__init__("{0}: {1}".format(filename, text), detail=detail)


class UserParseException(UserFileException):
    """ An exception related to the parsing of a specific file. """

    def __init__(self, filename, *, detail=None):
        """ Initialise the exception. """

        super().__init__(filename, "unable to parse file", detail=detail)


def handle_exception(e):
    """ Tell the user about an exception. """

    if isinstance(e, UserException):
        # An "expected" exception.
        if e.detail is not None:
            message = "{0}: {1}".format(e.text, e.detail)
        else:
            message = e.text

        print("{0}: {1}".format(os.path.basename(sys.argv[0]), message),
                file=sys.stderr)

        sys.exit(1)

    # An internal error.
    print(
            "{0}: An internal error occurred...".format(
                    os.path.basename(sys.argv[0])),
            file=sys.stderr)

    raise e


def deprecated(thing, *, filename=None, line_nr=None, instead=None):
    """ Tell the user about a deprecation. """

    next_major_version = (SIP_VERSION >> 16) + 1

    if filename is not None:
        prefix = filename + ': '

        if line_nr is not None:
            prefix += f"line {line_nr}: "
    else:
        prefix = ''

    message = f"{prefix}{thing} is deprecated and will be removed in SIP v{next_major_version}.0.0"

    if instead is not None:
        message += f", use {instead} instead"

    if _deprecations_are_errors:
        raise UserException(message)

    print(message, file=sys.stderr)
