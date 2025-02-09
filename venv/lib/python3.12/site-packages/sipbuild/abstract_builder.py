# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from abc import ABC, abstractmethod

from .configurable import Configurable


class AbstractBuilder(Configurable, ABC):
    """ This specifies the API of a builder. """

    def __init__(self, project, **kwargs):
        """ Initialise the builder. """

        super().__init__()

        self.project = project

        self.initialise_options(kwargs)

    @abstractmethod
    def build(self):
        """ Build the project in-situ. """

    @abstractmethod
    def build_sdist(self, sdist_directory):
        """ Build an sdist for the project and return the name of the sdist
        file.
        """

    @abstractmethod
    def build_wheel(self, wheel_directory):
        """ Build a wheel for the project and return the name of the wheel
        file.
        """

    @abstractmethod
    def install(self):
        """ Install the project. """
