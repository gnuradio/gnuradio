#
# Copyright 2011-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import os
import re
import sys
import glob

########################################################################
# Strip comments from a c/cpp file.
# Input is code string, output is code string without comments.
# http://stackoverflow.com/questions/241327/python-snippet-to-remove-c-and-c-comments
########################################################################
def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return ""
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

########################################################################
# Split code into nested sections according to ifdef preprocessor macros
########################################################################
def split_into_nested_ifdef_sections(code):
    sections = list()
    section = ''
    header = 'text'
    in_section_depth = 0
    for i, line in enumerate(code.splitlines()):
        m = re.match('^(\s*)#(\s*)(\w+)(.*)$', line)
        line_is = 'normal'
        if m:
            p0, p1, fcn, stuff = m.groups()
            if fcn in ('if', 'ifndef', 'ifdef'): line_is = 'if'
            if fcn in ('else', 'elif'): line_is = 'else'
            if fcn in ('endif',): line_is = 'end'

        if line_is == 'if': in_section_depth += 1
        if line_is == 'end': in_section_depth -= 1

        if in_section_depth == 1 and line_is == 'if':
            sections.append((header, section))
            section = ''
            header = line
            continue

        if in_section_depth == 1 and line_is == 'else':
            sections.append((header, section))
            section = ''
            header = line
            continue

        if in_section_depth == 0 and line_is == 'end':
            sections.append((header, section))
            section = ''
            header = 'text'
            continue

        section += line + '\n'

    sections.append((header, section)) #and pack remainder into sections
    sections = [sec for sec in sections if sec[1].strip()] #filter empty sections

    #recurse into non-text sections to fill subsections
    for i, (header, section) in enumerate(sections):
        if header == 'text': continue
        sections[i] = (header, split_into_nested_ifdef_sections(section))

    return sections

########################################################################
# Recursive print of sections to test code above
########################################################################
def print_sections(sections, indent = '  '):
    for header, body in sections:
        if header == 'text':
            print indent, ('\n'+indent).join(body.splitlines())
            continue
        print indent.replace(' ', '-') + '>', header
        print_sections(body, indent + '  ')

########################################################################
# Flatten a section to just body text
########################################################################
def flatten_section_text(sections):
    output = ''
    for hdr, bdy in sections:
        if hdr != 'text': output += flatten_section_text(bdy)
        else: output += bdy
    return output

########################################################################
# Extract kernel info from section, represent as an implementation
########################################################################
class impl_class:
    def __init__(self, kern_name, header, body):
        #extract LV_HAVE_*
        self.deps = set(map(str.lower, re.findall('LV_HAVE_(\w+)', header)))
        #extract function suffix and args
        body = flatten_section_text(body)
        try:
            fcn_matcher = re.compile('^.*(%s\\w*)\\s*\\((.*)$'%kern_name, re.DOTALL | re.MULTILINE)
            body = body.split('{')[0].rsplit(')', 1)[0] #get the part before the open ){ bracket
            m = fcn_matcher.match(body)
            impl_name, the_rest = m.groups()
            self.name = impl_name.replace(kern_name+'_', '')
            self.args = list()
            fcn_args = the_rest.split(',')
            for fcn_arg in fcn_args:
                arg_matcher = re.compile('^\s*(.*\\W)\s*(\w+)\s*$', re.DOTALL | re.MULTILINE)
                m = arg_matcher.match(fcn_arg)
                arg_type, arg_name = m.groups()
                self.args.append((arg_type, arg_name))
        except Exception as ex:
            raise Exception, 'I cant parse the function prototype from: %s in %s\n%s'%(kern_name, body, ex)

        assert self.name
        self.is_aligned = self.name.startswith('a_')

    def __repr__(self):
        return self.name

########################################################################
# Get sets of LV_HAVE_* from the code
########################################################################
def extract_lv_haves(code):
    haves = list()
    for line in code.splitlines():
        if not line.strip().startswith('#'): continue
        have_set = set(map(str.lower, re.findall('LV_HAVE_(\w+)', line)))
        if have_set: haves.append(have_set)
    return haves

########################################################################
# Represent a processing kernel, parse from file
########################################################################
class kernel_class:
    def __init__(self, kernel_file):
        self.name = os.path.splitext(os.path.basename(kernel_file))[0]
        self.pname = self.name.replace('volk_', 'p_')
        code = open(kernel_file, 'r').read()
        code = comment_remover(code)
        sections = split_into_nested_ifdef_sections(code)
        self._impls = list()
        for header, section in sections:
            if 'ifndef' not in header.lower(): continue
            for sub_hdr, body in section:
                if 'if' not in sub_hdr.lower(): continue
                if 'LV_HAVE_' not in sub_hdr: continue
                self._impls.append(impl_class(
                    kern_name=self.name, header=sub_hdr, body=body,
                ))
        assert(self._impls)
        self.has_dispatcher = False
        for impl in self._impls:
            if impl.name == 'dispatcher':
                self._impls.remove(impl)
                self.has_dispatcher = True
                break
        self.args = self._impls[0].args
        self.arglist_types = ', '.join([a[0] for a in self.args])
        self.arglist_full = ', '.join(['%s %s'%a for a in self.args])
        self.arglist_names = ', '.join([a[1] for a in self.args])

    def get_impls(self, archs):
        archs = set(archs)
        impls = list()
        for impl in self._impls:
            if impl.deps.intersection(archs) == impl.deps:
                impls.append(impl)
        return impls

    def __repr__(self):
        return self.name

########################################################################
# Extract information from the VOLK kernels
########################################################################
__file__ = os.path.abspath(__file__)
srcdir = os.path.dirname(os.path.dirname(__file__))
kernel_files = glob.glob(os.path.join(srcdir, "kernels", "volk", "*.h"))
kernels = map(kernel_class, kernel_files)

if __name__ == '__main__':
    print kernels
