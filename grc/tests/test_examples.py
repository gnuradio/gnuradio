# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import pytest

from argparse import Namespace
from os import path
import tempfile
from pathlib import Path

from grc.compiler import main

def run_compiler(capsys, pathname):
    try:
        args = Namespace(
            output=tempfile.gettempdir(),
            user_lib_dir=False,
            grc_files=[pathname],
            run=False
        )

        main(args)
        out, err = capsys.readouterr()
    except:
        err = "Exception in running compiler"
   
    return err
        

def test_compile_all_examples(capsys):
    topdir = path.join(path.dirname(__file__),"../..")
    # Grab all the .grc files in the source tree that live under "examples" dirs
    result = list(Path(topdir).rglob("*.grc"))
    result = [x for x in result if 'examples' in str(x)]
    err_stdout = []
    err_fnames = []
    for r in result:
        ret = run_compiler(capsys, str(r))
        if ret:
            err_stdout.append(ret)
            err_fnames.append(str(r))
    
    if (err_fnames):
        print("Failing .grc files:")
        for (fn,err) in zip(err_fnames, err_stdout):
            print(fn)
            print(err)

        assert(False)
