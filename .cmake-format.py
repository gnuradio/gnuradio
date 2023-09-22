# Copyright 2021 Marcus Müller
# SPDX-License-Identifier: LGPL-3.0-or-later
from tools import clang_format_helper
_clang_format = clang_format_helper.clang_format_options()

with section("parse"):
    additional_commands = {
        'gr_python_install': {
            'flags': [],
            'kwargs': {
                "PROGRAMS": "*",
                "FILES": "*",
                "DESTINATION": "*"
            }
        },
    }
with section("markup"):
    first_comment_is_literal = True
    enable_markup = False
with section("format"):
    disable = False

    line_width = int(_clang_format["ColumnLimit"])
    tab_size = int(_clang_format["IndentWidth"])
    min_prefix_chars = tab_size
    max_prefix_chars = 3 * tab_size
    use_tabchars = _clang_format["UseTab"] in ("ForIndentation",
                                               "ForContinuationAndIndentation",
                                               "Always")

    # separate_ctrl_name_with_space = clang_format["SpaceBeforeParens"] in (
    #     "ControlStatements", "ControlStatementsExceptControlMacros", "Always")
    separate_ctrl_name_with_space = False
    # separate_fn_name_with_space = clang_format["SpaceBeforeParens"] != "Never"
    separate_fn_name_with_space = False
    dangle_parens = False

    command_case = 'canonical'
    keyword_case = 'upper'

with section("lint"):
    max_arguments = 6
    max_localvars = 20
    max_statements = 75
