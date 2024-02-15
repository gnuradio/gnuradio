#!/usr/bin/env python
#
# Copyright 2019,2020 Free Software Foundation, Inc.
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
import os


class test_prefs (gr_unittest.TestCase):

    def test_001(self):
        p = gr.prefs()

        # Read some options
        self.assertFalse(p.has_option('doesnt', 'exist'))

        # At the time these tests are run, there is not necessarily a default
        # configuration on the build system, so not much to do with testing
        # here

    def _test_env(self, env: str, subdir: [str, None] = None):
        import tempfile
        oldenv = os.getenv(env, "")
        tmpdir = tempfile.mkdtemp()
        if subdir:
            os.mkdir(os.path.join(tmpdir, subdir))
            conffile = os.path.join(tmpdir, subdir, "config.conf")
        else:
            conffile = os.path.join(tmpdir, "config.conf")
        f = open(conffile, "w", encoding="utf-8")
        f.write("[TEST_SECTION]\ncanary=alive\n")
        f.close()
        os.environ[env] = tmpdir
        prefs = gr.prefs()
        self.assertEqual(prefs.get_string("TEST_SECTION", "canary", "verydead"), "alive")
        magicvalue = 1337
        prefs.set_long("WRITE_SECTION", "value", magicvalue)
        self.assertEqual(prefs.get_long("WRITE_SECTION", "value", 0), magicvalue)
        self.assertEqual(prefs.get_string("TEST_SECTION", "canary", "verydead"), "alive")
        os.unlink(conffile)
        if subdir:
            os.rmdir(os.path.join(tmpdir, subdir))
        os.rmdir(tmpdir)
        if oldenv:
            os.putenv(env, oldenv)
        else:
            os.unsetenv(env)

    def test_002_react_to_GR_PREFS_PATH(self):
        self._test_env("GR_PREFS_PATH")

    def test_003_react_to_XDG_CONFIG_HOME(self):
        oldgrprefspath = os.getenv("GR_PREFS_PATH", None)
        if oldgrprefspath:
            os.unsetenv("GR_PREFS_PATH")
        self._test_env("XDG_CONFIG_HOME", "gnuradio")
        if oldgrprefspath:
            os.putenv("GR_PREFS_PATH", oldgrprefspath)


if __name__ == '__main__':
    gr_unittest.run(test_prefs)
