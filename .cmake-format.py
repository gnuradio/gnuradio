# Copyright 2021 Marcus Müller
# SPDX-License-Identifier: LGPL-3.0-or-later
class _clang_format_options:
    def __init__(self, clangfile=None):
        if not clangfile:
            clangfile = ".clang-format"
        self.lines = []
        with open(clangfile, encoding="utf-8") as opened:
            for line in opened:
                if line.strip().startswith("#"):
                    continue
                self.lines.append(line.rstrip().split(":"))

    def __getitem__(self, string):
        path = string.split(".")
        value = None
        for crumble in path:
            for line in self.lines:
                if line[0].strip() == crumble:
                    if len(line) > 1:
                        value = line[1].strip().rstrip()
                    break
        return value


_clang_format = _clang_format_options()

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
    separate_ctrl_name_with_space = False
    separate_fn_name_with_space = False
    dangle_parens = False
    command_case = 'canonical'
    keyword_case = 'upper'

with section("lint"):
    max_arguments = 6
    max_localvars = 20
    max_statements = 75
