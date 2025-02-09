# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from distutils.command.build_ext import build_ext
from distutils.dist import Distribution
from distutils.extension import Extension
from distutils.log import ERROR, INFO, set_threshold

import os

from .buildable import BuildableModule
from .builder import Builder
from .exceptions import UserException
from .installable import Installable


class DistutilsBuilder(Builder):
    """ The implementation of a distutils-based project builder. """

    def build_executable(self, buildable, *, fatal=True):
        """ Build an executable from a BuildableExecutable object and return
        the relative pathname of the executable.
        """

        raise UserException("DistutilsBuilder cannot build executables")

    def build_project(self, target_dir, *, wheel_tag=None):
        """ Build the project. """

        project = self.project

        # On macOS respect the minimum macOS version.
        remove_macos_target = False

        if project.py_platform == 'darwin':
            # If the target version has already been set then assume the user
            # knows what they are doing and leave it as it is.
            if 'MACOSX_DEPLOYMENT_TARGET' not in os.environ:
                if project.minimum_macos_version:
                    macos_target = '.'.join(
                            [str(v) for v in project.minimum_macos_version])
                    os.environ['MACOSX_DEPLOYMENT_TARGET'] = macos_target
                    remove_macos_target = True

        # Build the buildables.
        for buildable in project.buildables:
            if isinstance(buildable, BuildableModule):
                if buildable.static:
                    raise UserException(
                            "DistutilsBuilder cannot build static modules")

                self._build_extension_module(buildable)
            else:
                raise UserException(
                        "DistutilsBuilder cannot build '{0}' buildables".format(
                                type(buildable).__name__))

        # Tidy up.
        if remove_macos_target:
            del os.environ['MACOSX_DEPLOYMENT_TARGET']

    def install_project(self, target_dir, *, wheel_tag=None):
        """ Install the project into a target directory. """

        project = self.project

        installed = []

        # Install any project-level installables.
        for installable in project.installables:
            installable.install(target_dir, installed)

        # Install any installables from built buildables.
        for buildable in project.buildables:
            for installable in buildable.installables:
                installable.install(target_dir, installed)

        if project.distinfo:
            from .distinfo import create_distinfo

            create_distinfo(project.get_distinfo_dir(target_dir), wheel_tag,
                    installed, project.metadata, project.get_requires_dists(),
                    project.root_dir, project.console_scripts,
                    project.gui_scripts)

    def _build_extension_module(self, buildable):
        """ Build an extension module from the sources. """

        project = self.project

        set_threshold(INFO if project.verbose else ERROR)

        distribution = Distribution()

        module_builder = ExtensionCommand(distribution, buildable)
        module_builder.build_lib = buildable.build_dir
        module_builder.debug = buildable.debug

        if buildable.debug:
            # Enable assert().
            module_builder.undef = 'NDEBUG'

        module_builder.ensure_finalized()

        # Convert the #defines.
        define_macros = []
        for macro in buildable.define_macros:
            parts = macro.split('=', maxsplit=1)
            name = parts[0]
            try:
                value = parts[1]
            except IndexError:
                value = None

            define_macros.append((name, value))

        buildable.make_names_relative()

        module_builder.extensions = [
            Extension(buildable.fq_name, buildable.sources,
                    define_macros=define_macros,
                    extra_compile_args=buildable.extra_compile_args,
                    extra_link_args=buildable.extra_link_args,
                    extra_objects=buildable.extra_objects,
                    include_dirs=buildable.include_dirs,
                    libraries=buildable.libraries,
                    library_dirs=buildable.library_dirs)]

        project.progress(
                "Compiling the '{0}' module".format(buildable.fq_name))

        saved_cwd = os.getcwd()
        os.chdir(buildable.build_dir)

        try:
            module_builder.run()
        except Exception as e:
            raise UserException(
                    "Unable to compile the '{0}' module".format(
                            buildable.fq_name),
                    detail=str(e))

        # Add the extension module to the buildable's list of installables.
        installable = Installable('module',
                target_subdir=buildable.get_install_subdir())
        installable.files.append(
                module_builder.get_ext_fullpath(buildable.fq_name))
        buildable.installables.append(installable)

        os.chdir(saved_cwd)


class ExtensionCommand(build_ext):
    """ Extend the distutils command to build an extension module. """

    def __init__(self, distribution, buildable):
        """ Initialise the object. """

        super().__init__(distribution)

        self._buildable = buildable

    def get_ext_filename(self, ext_name):
        """ Reimplemented to handle modules that use the limited API. """

        return os.path.join(*ext_name.split('.')) + self._buildable.get_module_extension()
