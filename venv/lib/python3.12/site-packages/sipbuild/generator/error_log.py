# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ..exceptions import UserException


class ErrorLog:
    """ This object logs errors and raises a corresponding exception when
    requested.
    """

    def __init__(self):
        """ Initialise the error log. """

        self._errors = []

    def log(self, text, source_location=None):
        """ Log an error with an optional source location. """

        # Format the entry.
        if source_location is None:
            entry = ''
        else:
            entry = source_location.sip_file + ": "

            if source_location.line > 0:
                entry += "line {0}: ".format(source_location.line)

                if source_location.column > 0:
                    entry += "column {0}: ".format(source_location.column)

        entry += text

        self._errors.append(entry)

    def as_exception(self):
        """ Raise a UserException for any logged errors. """

        if self._errors:
            raise UserException('\n'.join(self._errors))
