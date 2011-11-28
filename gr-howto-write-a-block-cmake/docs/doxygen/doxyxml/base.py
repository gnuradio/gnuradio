#
# Copyright 2010 Free Software Foundation, Inc.
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
"""
A base class is created.

Classes based upon this are used to make more user-friendly interfaces
to the doxygen xml docs than the generated classes provide.
"""

import os
import pdb

from xml.parsers.expat import ExpatError

from generated import compound


class Base(object):

    class Duplicate(StandardError):
        pass

    class NoSuchMember(StandardError):
        pass

    class ParsingError(StandardError):
        pass

    def __init__(self, parse_data, top=None):
        self._parsed = False
        self._error = False
        self._parse_data = parse_data
        self._members = []
        self._dict_members = {}
        self._in_category = {}
        self._data = {}
        if top is not None:
            self._xml_path = top._xml_path
            # Set up holder of references
        else:
            top = self
            self._refs = {}
            self._xml_path = parse_data
        self.top = top

    @classmethod
    def from_refid(cls, refid, top=None):
        """ Instantiate class from a refid rather than parsing object. """
        # First check to see if its already been instantiated.
        if top is not None and refid in top._refs:
            return top._refs[refid]
        # Otherwise create a new instance and set refid.
        inst = cls(None, top=top)
        inst.refid = refid
        inst.add_ref(inst)
        return inst

    @classmethod
    def from_parse_data(cls, parse_data, top=None):
        refid = getattr(parse_data, 'refid', None)
        if refid is not None and top is not None and refid in top._refs:
            return top._refs[refid]
        inst = cls(parse_data, top=top)
        if refid is not None:
            inst.refid = refid
            inst.add_ref(inst)
        return inst

    def add_ref(self, obj):
        if hasattr(obj, 'refid'):
            self.top._refs[obj.refid] = obj

    mem_classes = []

    def get_cls(self, mem):
        for cls in self.mem_classes:
            if cls.can_parse(mem):
                return cls
        raise StandardError(("Did not find a class for object '%s'." \
                                 % (mem.get_name())))

    def convert_mem(self, mem):
        try:
            cls = self.get_cls(mem)
            converted = cls.from_parse_data(mem, self.top)
            if converted is None:
                raise StandardError('No class matched this object.')
            self.add_ref(converted)
            return converted
        except StandardError, e:
            print e

    @classmethod
    def includes(cls, inst):
        return isinstance(inst, cls)

    @classmethod
    def can_parse(cls, obj):
        return False

    def _parse(self):
        self._parsed = True

    def _get_dict_members(self, cat=None):
        """
        For given category a dictionary is returned mapping member names to
        members of that category.  For names that are duplicated the name is
        mapped to None.
        """
        self.confirm_no_error()
        if cat not in self._dict_members:
            new_dict = {}
            for mem in self.in_category(cat):
                if mem.name() not in new_dict:
                    new_dict[mem.name()] = mem
                else:
                    new_dict[mem.name()] = self.Duplicate
            self._dict_members[cat] = new_dict
        return self._dict_members[cat]

    def in_category(self, cat):
        self.confirm_no_error()
        if cat is None:
            return self._members
        if cat not in self._in_category:
            self._in_category[cat] = [mem for mem in self._members
                                      if cat.includes(mem)]
        return self._in_category[cat]
        
    def get_member(self, name, cat=None):
        self.confirm_no_error()
        # Check if it's in a namespace or class.
        bits = name.split('::')
        first = bits[0]
        rest = '::'.join(bits[1:])
        member = self._get_dict_members(cat).get(first, self.NoSuchMember)
        # Raise any errors that are returned.
        if member in set([self.NoSuchMember, self.Duplicate]):
            raise member()
        if rest:
            return member.get_member(rest, cat=cat)
        return member

    def has_member(self, name, cat=None):
        try:
            mem = self.get_member(name, cat=cat)
            return True
        except self.NoSuchMember:
            return False

    def data(self):
        self.confirm_no_error()
        return self._data

    def members(self):
        self.confirm_no_error()
        return self._members
    
    def process_memberdefs(self):
        mdtss = []
        for sec in self._retrieved_data.compounddef.sectiondef:
            mdtss += sec.memberdef
        # At the moment we lose all information associated with sections.
        # Sometimes a memberdef is in several sectiondef.
        # We make sure we don't get duplicates here.
        uniques = set([])
        for mem in mdtss:
            converted = self.convert_mem(mem)
            pair = (mem.name, mem.__class__)
            if pair not in uniques:
                uniques.add(pair)
                self._members.append(converted)
        
    def retrieve_data(self):
        filename = os.path.join(self._xml_path, self.refid + '.xml')
        try:
            self._retrieved_data = compound.parse(filename)
        except ExpatError:
            print('Error in xml in file %s' % filename)
            self._error = True
            self._retrieved_data = None
            
    def check_parsed(self):
        if not self._parsed:
            self._parse()

    def confirm_no_error(self):
        self.check_parsed()
        if self._error:
            raise self.ParsingError()

    def error(self):
        self.check_parsed()
        return self._error

    def name(self):
        # first see if we can do it without processing.
        if self._parse_data is not None:
            return self._parse_data.name
        self.check_parsed()
        return self._retrieved_data.compounddef.name
