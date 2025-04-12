#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
''' A parser for blocks written in C++ '''


import re
import sys
import logging

logger = logging.getLogger(__name__)


def dummy_translator(the_type, default_v=None):
    """ Doesn't really translate. """
    return the_type


def remove_comments(inp: str) -> str:
    def comment_removal(match: re.match) -> str:
        groups = match.groups()
        if len(groups) == 1:
            return ''
        return inp
    try:
        out = re.sub(r'(/\*[^(\*/)]*\*/)', comment_removal, inp)
        logger.debug(f"out: {out}")
        return out
    except Exception as e:
        logger.error(f"error removing comments {e}")
        return inp


class ParserCCBlock(object):
    """ Class to read blocks written in C++ """

    def __init__(self, filename_cc, filename_h, blockname, version, type_trans=dummy_translator):
        with open(filename_cc) as f:
            self.code_cc = f.read()
        with open(filename_h) as f:
            self.code_h = f.read()
        self.blockname = blockname
        self.type_trans = type_trans
        self.version = version

    def read_io_signature(self):
        """ Scans a .cc file for an IO signature. """
        def _figure_out_iotype_and_vlen(iosigcall, typestr):
            """ From a type identifier, returns the data type.
            E.g., for sizeof(int), it will return 'int'.
            Returns a list! """
            if 'gr::io_signature::makev' in iosigcall:
                logger.error('tbi')
                raise ValueError
            return {'type': [_typestr_to_iotype(x) for x in typestr.split(',')],
                    'vlen': [_typestr_to_vlen(x) for x in typestr.split(',')]
                    }

        def _typestr_to_iotype(typestr):
            """ Convert a type string (e.g. sizeof(int) * vlen) to the type (e.g. 'int'). """
            type_match = re.search(r'sizeof\s*\(([^)]*)\)', typestr)
            if type_match is None:
                return self.type_trans('char')
            return self.type_trans(remove_comments(type_match.group(1)))

        def _typestr_to_vlen(typestr):
            """ From a type identifier, returns the vector length of the block's
            input/out. E.g., for 'sizeof(int) * 10', it returns 10. For
            'sizeof(int)', it returns '1'. For 'sizeof(int) * vlen', it returns
            the string vlen. """
            # Catch fringe case where no sizeof() is given
            if typestr.find('sizeof') == -1:
                return typestr
            if typestr.find('*') == -1:
                return '1'
            vlen_parts = typestr.split('*')
            for fac in vlen_parts:
                if fac.find('sizeof') != -1:
                    vlen_parts.remove(fac)
            if len(vlen_parts) == 1:
                return vlen_parts[0].strip()
            elif len(vlen_parts) > 1:
                return '*'.join(vlen_parts).strip()
        iosig = {}
        iosig_regex = r'(?P<incall>(gr::)?io_signature::make[23v]?)\s*\(\s*(?P<inmin>[^,]+),\s*(?P<inmax>[^,]+),' + \
                      r'\s*(?P<intype>(\([^\)]*\)|[^)])+)\),\s*' + \
                      r'(?P<outcall>(gr::)?io_signature::make[23v]?)\s*\(\s*(?P<outmin>[^,]+),\s*(?P<outmax>[^,]+),' + \
                      r'\s*(?P<outtype>(\([^\)]*\)|[^)])+)\)'
        iosig_match = re.compile(
            iosig_regex, re.MULTILINE).search(self.code_cc)

        try:
            iosig['in'] = _figure_out_iotype_and_vlen(iosig_match.group('incall'),
                                                      iosig_match.group('intype'))
            iosig['in']['min_ports'] = remove_comments(iosig_match.group('inmin'))
            iosig['in']['max_ports'] = remove_comments(iosig_match.group('inmax'))
        except Exception as e:
            logger.error(f"Error: Can't parse input signature: {e}")
        try:
            iosig['out'] = _figure_out_iotype_and_vlen(iosig_match.group('outcall'),
                                                       iosig_match.group('outtype'))
            iosig['out']['min_ports'] = remove_comments(iosig_match.group('outmin'))
            iosig['out']['max_ports'] = remove_comments(iosig_match.group('outmax'))
        except Exception as e:
            logger.error(f"Error: Can't parse output signature: {e}")
        return iosig

    def read_params(self):
        """ Read the parameters required to initialize the block """
        def _scan_param_list(start_idx):
            """ Go through a parameter list and return a tuple each:
                (type, name, default_value). Python's re just doesn't cut
                it for C++ code :( """
            i = start_idx
            c = self.code_h
            if c[i] != '(':
                raise ValueError
            i += 1

            param_list = []
            read_state = 'type'
            in_string = False
            parens_count = 0  # Counts ()
            brackets_count = 0  # Counts <>
            end_of_list = False
            this_type = ''
            this_name = ''
            this_defv = ''
            WHITESPACE = ' \t\n\r\f\v'
            while not end_of_list:
                # Keep track of (), stop when reaching final closing parens
                if not in_string:
                    if c[i] == ')':
                        if parens_count == 0:
                            if read_state == 'type' and len(this_type):
                                raise ValueError(
                                    'Found closing parentheses before finishing '
                                    'last argument (this is how far I got: {})'.format
                                    (str(param_list))
                                )
                            if len(this_type):
                                param_list.append(
                                    (this_type, this_name, this_defv))
                            end_of_list = True
                            break
                        else:
                            parens_count -= 1
                    elif c[i] == '(':
                        parens_count += 1
                # Parameter type (int, const std::string, std::vector<gr_complex>, unsigned long ...)
                if read_state == 'type':
                    if c[i] == '<':
                        brackets_count += 1
                    if c[i] == '>':
                        brackets_count -= 1
                    if c[i] == '&':
                        i += 1
                        continue
                    if c[i] in WHITESPACE and brackets_count == 0:
                        while c[i] in WHITESPACE:
                            i += 1
                            continue
                        if this_type == 'const' or this_type == '':  # Ignore this
                            this_type = ''
                        elif this_type == 'unsigned':  # Continue
                            this_type += ' '
                            continue
                        else:
                            read_state = 'name'
                        continue
                    this_type += c[i]
                    i += 1
                    continue
                # Parameter name
                if read_state == 'name':
                    if c[i] == '&' or c[i] in WHITESPACE:
                        i += 1
                    elif c[i] == '=':
                        if parens_count != 0:
                            raise ValueError(
                                'While parsing argument {} ({}): name finished but no closing parentheses.'.format
                                (len(param_list) + 1, this_type + ' ' + this_name)
                            )
                        read_state = 'defv'
                        i += 1
                    elif c[i] == ',':
                        if parens_count:
                            raise ValueError(
                                'While parsing argument {} ({}): name finished but no closing parentheses.'.format
                                (len(param_list) + 1, this_type + ' ' + this_name)
                            )
                        read_state = 'defv'
                    else:
                        this_name += c[i]
                        i += 1
                    continue
                # Default value
                if read_state == 'defv':
                    if in_string:
                        if c[i] == '"' and c[i - 1] != '\\':
                            in_string = False
                        else:
                            this_defv += c[i]
                    elif c[i] == ',':
                        if parens_count:
                            raise ValueError(
                                'While parsing argument {} ({}): default value finished but no closing parentheses.'.format
                                (len(param_list) + 1, this_type + ' ' + this_name)
                            )
                        read_state = 'type'
                        param_list.append((this_type, this_name, this_defv))
                        this_type = ''
                        this_name = ''
                        this_defv = ''
                    else:
                        this_defv += c[i]
                    i += 1
                    continue
            return param_list
        # Go, go, go!
        if self.version in ('37', '38', '39', '310'):
            make_regex = r'static\s+sptr\s+make\s*'
        else:
            make_regex = r'(?<=_API)\s+\w+_sptr\s+\w+_make_\w+\s*'
        make_match = re.compile(make_regex, re.MULTILINE).search(self.code_h)
        try:
            params_list = _scan_param_list(make_match.end(0))
        except ValueError as ve:
            logger.error("Can't parse the argument list: ", ve.args[0])
            sys.exit(0)
        params = []
        for plist in params_list:
            params.append({'type': self.type_trans(plist[0], plist[2]),
                           'key': plist[1],
                           'default': plist[2],
                           'in_constructor': True})
        return params
