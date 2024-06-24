#
# Copyright 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
The file for testing the gr-modtool CLI.
It's just a thin wrapper to allow calling a shell script.
"""


import os
import shutil
import tempfile
import unittest
import logging

import sys
from os import path
import subprocess


class TestModToolCli(unittest.TestCase):
    """ The tests for the modtool CLI """

    def __init__(self, *args, **kwargs):
        super(TestModToolCli, self).__init__(*args, **kwargs)
        self.python = sys.executable
        self.srcdir = path.abspath(path.join(path.dirname(
            path.realpath(__file__)), '../'))

        self.modname = "testmod"
        self.blockname = "tunnelpy"
        self.newname = "domythesis"

    @classmethod
    def setUpClass(cls):
        """ create a temporary directory """
        cls.start_dir = os.getcwd()
        cls.test_dir = tempfile.mkdtemp()

    @classmethod
    def tearDownClass(cls):
        """ remove the directory after the test """
        os.chdir(cls.start_dir)
        shutil.rmtree(cls.test_dir)

    def setUp(self):
        """ create a new module and block before every test """

    def tearDown(self):
        """ removes the created module """
        # Required, else the new-module directory command
        # in setup will throw exception after first test
        # cannot remove if directory is not created
        os.chdir(self.start_dir)
        rmdir = f"{self.test_dir}/gr-{self.modname}"
        shutil.rmtree(rmdir)

    def test_001_run_shell_script(self):
        """ Tests for the API function newmod """
        result = subprocess.run(
            [path.join(self.srcdir, "tests", "test_001_modtool_batch_cli.sh"),
             self.python,
             self.srcdir,
             self.modname,
             self.blockname,
             self.newname,
             self.test_dir],
            capture_output=True,
        )
        if result.returncode:
            raise Exception(str(result.stderr))


if __name__ == '__main__':
    unittest.main()
