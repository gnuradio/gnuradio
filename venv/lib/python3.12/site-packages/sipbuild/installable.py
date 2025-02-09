# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


import os
from shutil import copy2, copytree


class Installable:
    """ Encapsulate a list of files and directories that will be installed into
    a target directory.
    """

    def __init__(self, name, *, target_subdir=None):
        """ Initialise the installable.  The optional target_subdir is the
        path of a sub-directory of the eventual target where the files will be
        installed.  If target_subdir is an absolute pathname then it is used as
        the eventual target name.
        """

        self.name = name
        self.target_subdir = target_subdir
        self.files = []

    def get_full_target_dir(self, target_dir):
        """ Return the full target directory name. """

        if self.target_subdir:
            if os.path.isabs(self.target_subdir):
                target_dir = self.target_subdir
            else:
                target_dir = os.path.join(target_dir, self.target_subdir)

        return target_dir

    def install(self, target_dir, installed, *, do_install=True):
        """ Optionally install the files in a target directory and update the
        given list of installed files.
        """

        target_dir = self.get_full_target_dir(target_dir)

        if do_install:
            os.makedirs(target_dir, exist_ok=True)

        for fn in self.files:
            t_path = os.path.join(target_dir, os.path.basename(fn))
            installed.append(t_path)

            if do_install:
                copy_fn = copytree if os.path.isdir(fn) else copy2
                copy_fn(fn, t_path)
