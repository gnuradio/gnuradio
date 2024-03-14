#!/usr/bin/env python
#
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
import os
import tempfile


class test_prefs(gr_unittest.TestCase):

    def _test_env(self, env: str, subdir: [str, None] = None):
        oldenv = os.getenv(env, "")
        tmpdir = tempfile.mkdtemp()
        targetdir = tmpdir
        if subdir:
            targetdir = os.path.join(tmpdir, subdir)
            os.mkdir(targetdir)
        os.environ[env] = targetdir
        self.assertEqual(str(gr.paths.userconf()), targetdir)
        if subdir:
            os.rmdir(targetdir)
        os.rmdir(tmpdir)
        if oldenv:
            os.putenv(env, oldenv)
        else:
            os.unsetenv(env)

    def test_001_members(self):
        self.assertTrue(gr.paths.tmp())
        self.assertTrue(gr.paths.appdata())
        self.assertTrue(gr.paths.userconf())

    def test_002_react_to_GR_PREFS_PATH(self):
        self._test_env("GR_PREFS_PATH")

    def test_003_ensure_directory(self):
        import shutil

        cwd = os.getcwd()
        self.assertTrue(gr.paths.utilities.ensure_directory(cwd))
        tmpdir = tempfile.mkdtemp()
        self.assertTrue(gr.paths.utilities.ensure_directory(tmpdir))
        components = (
            tmpdir,
            "in the jungle the mighty jungle",
            "the lion sleeps tonight",
        )
        target = os.path.join(*components)
        self.assertFalse(os.path.exists(target))
        self.assertFalse(gr.paths.utilities.ensure_directory(target))
        self.assertTrue(gr.paths.utilities.ensure_directory(target))
        self.assertTrue(os.path.exists(target))
        shutil.rmtree(tmpdir)
        self.assertFalse(os.path.exists(target))


if __name__ == "__main__":
    gr_unittest.run(test_prefs)
