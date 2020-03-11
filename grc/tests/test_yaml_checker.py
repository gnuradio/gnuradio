# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
# 

import yaml

from grc.core.schema_checker import Validator, BLOCK_SCHEME


BLOCK1 = """
id: block_key
label: testname

parameters:
-   id: vlen
    label: Vec Length
    dtype: int
    default: 1
-   id: out_type
    label: Vec Length
    dtype: string
    default: complex
-   id: a
    label: Alpha
    dtype: ${ out_type }
    default: '0'

inputs:
-   label: in
    domain: stream
    dtype: complex
    vlen: ${ 2 * vlen }
-   name: in2
    domain: message
    id: in2

outputs:
-   label: out
    domain: stream
    dtype: ${ out_type }
    vlen: ${ vlen }

templates:
    make: blocks.complex_to_mag_squared(${ vlen })
    
file_format: 1
"""


def test_min():
    checker = Validator(BLOCK_SCHEME)
    assert checker.run({'id': 'test', 'file_format': 1}), checker.messages
    assert not checker.run({'name': 'test', 'file_format': 1})


def test_extra_keys():
    checker = Validator(BLOCK_SCHEME)
    assert checker.run({'id': 'test', 'abcdefg': 'nonsense', 'file_format': 1})
    assert checker.messages == [('block', 'warn', "Ignoring extra key 'abcdefg'")]


def test_checker():
    checker = Validator(BLOCK_SCHEME)
    data = yaml.safe_load(BLOCK1)
    passed = checker.run(data)
    if not passed:
        print()
        for msg in checker.messages:
            print(msg)

    assert passed, checker.messages
