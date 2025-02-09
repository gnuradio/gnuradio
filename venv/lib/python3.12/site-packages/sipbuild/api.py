# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from .abstract_project import AbstractProject
from .exceptions import handle_exception


def build_sdist(sdist_directory, config_settings=None):
    """ The PEP 517 hook for building an sdist from pyproject.toml. """

    project = AbstractProject.bootstrap('sdist',
            arguments=_convert_config_settings(config_settings))

    # pip executes this in a separate process and doesn't handle exceptions
    # very well.  However it does capture stdout and (eventually) show it to
    # the user so we use our standard exception handling.
    try:
        return project.build_sdist(sdist_directory)
    except Exception as e:
        handle_exception(e)


def build_wheel(wheel_directory, config_settings=None, metadata_directory=None):
    """ The PEP 517 hook for building a wheel from pyproject.toml. """

    project = AbstractProject.bootstrap('wheel',
            arguments=_convert_config_settings(config_settings))

    # pip executes this in a separate process and doesn't handle exceptions
    # very well.  However it does capture stdout and (eventually) show it to
    # the user so we use our standard exception handling.
    try:
        return project.build_wheel(wheel_directory)
    except Exception as e:
        handle_exception(e)


def _convert_config_settings(config_settings):
    """ Return any configuration settings from the frontend to a pseudo-command
    line.
    """

    if config_settings is None:
        config_settings = {}

    args = []

    for name, value in config_settings.items():
        if value:
            if not isinstance(value, list):
                value = [value]

            for m_value in value:
                args.append(name + '=' + m_value)
        else:
            args.append(name)

    return args
