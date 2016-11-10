"""
Copyright 2008, 2009, 2015 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""


class Element(object):

    def __init__(self, parent=None):
        self._parent = parent
        self._error_messages = list()

    ##################################################
    # Element Validation API
    ##################################################
    def validate(self):
        """
        Validate this element and call validate on all children.
        Call this base method before adding error messages in the subclass.
        """
        del self._error_messages[:]
        for child in self.get_children():
            child.validate()

    def is_valid(self):
        """
        Is this element valid?

        Returns:
            true when the element is enabled and has no error messages or is bypassed
        """
        return (not self.get_error_messages() or not self.get_enabled()) or self.get_bypassed()

    def add_error_message(self, msg):
        """
        Add an error message to the list of errors.

        Args:
            msg: the error message string
        """
        self._error_messages.append(msg)

    def get_error_messages(self):
        """
        Get the list of error messages from this element and all of its children.
        Do not include the error messages from disabled or bypassed children.
        Cleverly indent the children error messages for printing purposes.

        Returns:
            a list of error message strings
        """
        error_messages = list(self._error_messages)  # Make a copy
        for child in filter(lambda c: c.get_enabled() and not c.get_bypassed(), self.get_children()):
            for msg in child.get_error_messages():
                error_messages.append("{0}:\n\t{1}".format(child, msg.replace("\n", "\n\t")))
        return error_messages

    def rewrite(self):
        """
        Rewrite this element and call rewrite on all children.
        Call this base method before rewriting the element.
        """
        for child in self.get_children():
            child.rewrite()

    def get_enabled(self):
        return True

    def get_bypassed(self):
        return False

    ##############################################
    # Tree-like API
    ##############################################
    def get_parent(self):
        return self._parent

    def get_children(self):
        return list()

    ##############################################
    # Type testing
    ##############################################
    is_platform = False

    is_flow_graph = False

    is_block = False

    is_dummy_block = False

    is_connection = False

    is_port = False

    is_param = False

    is_variable = False

    is_import = False
