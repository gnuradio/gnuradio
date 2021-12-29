# Copyright 2021 Marcus Müller
# SPDX-License-Identifier: LGPL-3.0-or-later
class clang_format_options:
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
