# Copyright 2008-2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import re
import builtins
import keyword
from typing import List, Callable

from .. import blocks
from .. import Constants


# Blacklist certain ids, its not complete, but should help
ID_BLACKLIST = {'self', 'gnuradio'} | set(dir(builtins)) | set(keyword.kwlist)
# Python >= 3.10 has soft keywords in a list, i.e. words that are reserved in
# specific contexts, e.g. `match` isn't generally a keyword, but in `case` it is.
ID_BLACKLIST |= set(getattr(keyword, 'softkwlist', set()))

try:
    from gnuradio import gr
    ID_BLACKLIST |= {attr for attr in dir(
        gr.top_block()) if not attr.startswith('_')}
except (ImportError, AttributeError):
    pass


validators = {}


def validates(*dtypes) -> Callable:
    """
    Registers a function as validator for the type of element give as strings
    """
    def decorator(func):
        for dtype in dtypes:
            assert dtype in Constants.PARAM_TYPE_NAMES
            validators[dtype] = func
        return func
    return decorator


class ValidateError(Exception):
    """Raised by validate functions"""


@validates('id')
def validate_block_id(param, black_listed_ids: List[str]) -> None:
    value = param.value
    # Can python use this as a variable?
    if not re.match(r'^[a-z|A-Z]\w*$', value):
        raise ValidateError('ID "{}" must begin with a letter and may contain letters, numbers, '
                            'and underscores.'.format(value))
    if (value in ID_BLACKLIST or value in black_listed_ids) \
            and not getattr(param.parent_block, 'exempt_from_id_validation', False):
        # Grant blacklist exemption to epy blocks and modules
        raise ValidateError('ID "{}" is blacklisted.'.format(value))
    block_names = [
        block.name for block in param.parent_flowgraph.iter_enabled_blocks()]
    # Id should only appear once, or zero times if block is disabled
    if param.key == 'id' and block_names.count(value) > 1:
        raise ValidateError('ID "{}" is not unique.'.format(value))
    elif value not in block_names:
        raise ValidateError('ID "{}" does not exist.'.format(value))


@validates('name')
def validate_name(param, _) -> None:
    # Name of a function or other block that will be generated literally not as a string
    value = param.value
    # Allow blank to pass validation
    # Can python use this as a variable?
    if not re.match(r'^([a-z|A-Z]\w*)?$', value):
        raise ValidateError('ID "{}" must begin with a letter and may contain letters, numbers, '
                            'and underscores.'.format(value))


@validates('stream_id')
def validate_stream_id(param, _) -> None:
    value = param.value
    stream_ids = [
        block.params['stream_id'].value
        for block in param.parent_flowgraph.iter_enabled_blocks()
        if isinstance(block, blocks.VirtualSink)
    ]
    # Check that the virtual sink's stream id is unique
    if isinstance(param.parent_block, blocks.VirtualSink) and stream_ids.count(value) >= 2:
        # Id should only appear once, or zero times if block is disabled
        raise ValidateError('Stream ID "{}" is not unique.'.format(value))
    # Check that the virtual source's steam id is found
    elif isinstance(param.parent_block, blocks.VirtualSource) and value not in stream_ids:
        raise ValidateError('Stream ID "{}" is not found.'.format(value))


@validates('complex', 'real', 'float', 'int')
def validate_scalar(param, _) -> None:
    valid_types = Constants.PARAM_TYPE_MAP[param.dtype]
    if not isinstance(param.get_evaluated(), valid_types):
        raise ValidateError('Expression {!r} is invalid for type {!r}.'.format(
            param.get_evaluated(), param.dtype))


@validates('complex_vector', 'real_vector', 'float_vector', 'int_vector')
def validate_vector(param, _) -> None:
    # todo: check vector types

    if param.get_evaluated() is None:
        raise ValidateError('Expression {!r} is invalid for type{!r}.'.format(
            param.get_evaluated(), param.dtype))

    valid_types = Constants.PARAM_TYPE_MAP[param.dtype.split('_', 1)[0]]
    if not all(isinstance(item, valid_types) for item in param.get_evaluated()):
        raise ValidateError('Expression {!r} is invalid for type {!r}.'.format(
            param.get_evaluated(), param.dtype))


@validates('gui_hint')
def validate_gui_hint(param, _) -> None:
    try:
        # Only parse the param if there are no errors
        if len(param.get_error_messages()) > 0:
            return
        param.parse_gui_hint(param.value)
    except Exception as e:
        raise ValidateError(str(e))
