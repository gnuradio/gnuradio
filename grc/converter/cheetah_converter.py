# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import collections
import re
import string

delims = {'(': ')', '[': ']', '{': '}', '': ', #\\*:'}
identifier_start = '_' + string.ascii_letters + ''.join(delims.keys())
string_delims = '"\''

cheetah_substitution = re.compile(
    r'^\$((?P<d1>\()|(?P<d2>\{)|(?P<d3>\[)|)'
    r'(?P<arg>[_a-zA-Z][_a-zA-Z0-9]*(?:\.[_a-zA-Z][_a-zA-Z0-9]*)?)(?P<eval>\(\))?'
    r'(?(d1)\)|(?(d2)\}|(?(d3)\]|)))$'
)
cheetah_inline_if = re.compile(
    r'#if (?P<cond>.*) then (?P<then>.*?) ?else (?P<else>.*?) ?(#|$)')


class Python(object):
    start = ''
    end = ''
    nested_start = ''
    nested_end = ''
    eval = ''
    type = str  # yaml_output.Eval


class FormatString(Python):
    start = '{'
    end = '}'
    nested_start = '{'
    nested_end = '}'
    eval = ':eval'
    type = str


class Mako(Python):
    start = '${'
    end = '}'
    nested_start = ''
    nested_end = ''
    type = str


class Converter(object):

    def __init__(self, names):
        self.stats = collections.defaultdict(int)
        self.names = set(names)
        self.extended = set(self._iter_identifiers(names))

    @staticmethod
    def _iter_identifiers(names):
        if not isinstance(names, dict):
            names = {name: {} for name in names}
        for key, sub_keys in names.items():
            yield key
            for sub_key in sub_keys:
                yield '{}.{}'.format(key, sub_key)

    def to_python(self, expr):
        return self.convert(expr=expr, spec=Python)

    def to_python_dec(self, expr):
        converted = self.convert(expr=expr, spec=Python)
        if converted and converted != expr:
            converted = '${ ' + converted.strip() + ' }'
        return converted

    def to_format_string(self, expr):
        return self.convert(expr=expr, spec=FormatString)

    def to_mako(self, expr):
        return self.convert(expr=expr, spec=Mako)

    def convert(self, expr, spec=Python):
        if not expr:
            return ''

        elif '$' not in expr:
            return expr

        try:
            return self.convert_simple(expr, spec)
        except ValueError:
            pass

        try:
            if '#if' in expr and '\n' not in expr:
                expr = self.convert_inline_conditional(expr, spec)
            return self.convert_hard(expr, spec)
        except ValueError:
            return 'Cheetah! ' + expr

    def convert_simple(self, expr, spec=Python):
        match = cheetah_substitution.match(expr)
        if not match:
            raise ValueError('Not a simple substitution: ' + expr)

        identifier = match.group('arg')
        if identifier not in self.extended:
            raise NameError('Unknown substitution {!r}'.format(identifier))
        if match.group('eval'):
            identifier += spec.eval

        out = spec.start + identifier + spec.end
        if '$' in out or '#' in out:
            raise ValueError('Failed to convert: ' + expr)

        self.stats['simple'] += 1
        return spec.type(out)

    def convert_hard(self, expr, spec=Python):
        lines = '\n'.join(self.convert_hard_line(line, spec)
                          for line in expr.split('\n'))
        if spec == Mako:
            # no line-continuation before a mako control structure
            lines = re.sub(r'\\\n(\s*%)', r'\n\1', lines)
        return lines

    def convert_hard_line(self, expr, spec=Python):
        if spec == Mako:
            if '#set' in expr:
                ws, set_, statement = expr.partition('#set ')
                return ws + '<% ' + self.to_python(statement) + ' %>'

            if '#if' in expr:
                ws, if_, condition = expr.partition('#if ')
                return ws + '% if ' + self.to_python(condition) + ':'
            if '#else if' in expr:
                ws, elif_, condition = expr.partition('#else if ')
                return ws + '% elif ' + self.to_python(condition) + ':'
            if '#else' in expr:
                return expr.replace('#else', '% else:')
            if '#end if' in expr:
                return expr.replace('#end if', '% endif')

            if '#slurp' in expr:
                expr = expr.split('#slurp', 1)[0] + '\\'
        return self.convert_hard_replace(expr, spec)

    def convert_hard_replace(self, expr, spec=Python):
        counts = collections.Counter()

        def all_delims_closed():
            for opener_, closer_ in delims.items():
                if counts[opener_] != counts[closer_]:
                    return False
            return True

        def extra_close():
            for opener_, closer_ in delims.items():
                if counts[opener_] < counts[closer_]:
                    return True
            return False

        out = []
        delim_to_find = False

        pos = 0
        char = ''
        in_string = None
        while pos < len(expr):
            prev, char = char, expr[pos]
            counts.update(char)

            if char in string_delims:
                if not in_string:
                    in_string = char
                elif char == in_string:
                    in_string = None
                    out.append(char)
                    pos += 1
                    continue
            if in_string:
                out.append(char)
                pos += 1
                continue

            if char == '$':
                pass  # no output

            elif prev == '$':
                if char not in identifier_start:  # not a substitution
                    out.append('$' + char)  # now print the $ we skipped over

                elif not delim_to_find:  # start of a substitution
                    try:
                        delim_to_find = delims[char]
                        out.append(spec.start)
                    except KeyError:
                        if char in identifier_start:
                            delim_to_find = delims['']
                            out.append(spec.start)
                            out.append(char)

                    counts.clear()
                    counts.update(char)

                else:  # nested substitution: simply match known variable names
                    found = False
                    for known_identifier in self.names:
                        if expr[pos:].startswith(known_identifier):
                            found = True
                            break
                    if found:
                        out.append(spec.nested_start)
                        out.append(known_identifier)
                        out.append(spec.nested_end)
                        pos += len(known_identifier)
                        continue

            elif delim_to_find and char in delim_to_find and all_delims_closed():  # end of substitution
                out.append(spec.end)
                if char in delims['']:
                    out.append(char)
                delim_to_find = False

            # end of substitution
            elif delim_to_find and char in ')]}' and extra_close():
                out.append(spec.end)
                out.append(char)
                delim_to_find = False

            else:
                out.append(char)

            pos += 1

        if delim_to_find == delims['']:
            out.append(spec.end)

        out = ''.join(out)
        # fix: eval stuff
        out = re.sub(r'(?P<arg>' + r'|'.join(self.extended) +
                     r')\(\)', r'\g<arg>', out)

        self.stats['hard'] += 1
        return spec.type(out)

    def convert_inline_conditional(self, expr, spec=Python):
        if spec == FormatString:
            raise ValueError('No conditionals in format strings: ' + expr)
        matcher = r'\g<then> if \g<cond> else \g<else>'
        if spec == Python:
            matcher = '(' + matcher + ')'
        expr = cheetah_inline_if.sub(matcher, expr)
        return spec.type(self.convert_hard(expr, spec))


class DummyConverter(object):

    def __init__(self, names={}):
        pass

    def to_python(self, expr):
        return expr

    def to_format_string(self, expr):
        return expr

    def to_mako(self, expr):
        return expr
