#
# Copyright 2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
Classes providing more user-friendly interfaces to the doxygen xml
docs than the generated classes provide.
"""

import os

from .generated import index
from .base import Base
from .text import description

class DoxyIndex(Base):
    """
    Parses a doxygen xml directory.
    """

    __module__ = "gnuradio.utils.doxyxml"

    def _parse(self):
        if self._parsed:
            return
        super(DoxyIndex, self)._parse()
        self._root = index.parse(os.path.join(self._xml_path, 'index.xml'))
        for mem in self._root.compound:
            converted = self.convert_mem(mem)
            # For files and namespaces we want the contents to be
            # accessible directly from the parent rather than having
            # to go through the file object.
            if self.get_cls(mem) == DoxyFile:
                if mem.name.endswith('.h'):
                    self._members += converted.members()
                    self._members.append(converted)
            elif self.get_cls(mem) == DoxyNamespace:
                self._members += converted.members()
                self._members.append(converted)
            else:
                self._members.append(converted)


class DoxyCompMem(Base):


    kind = None

    def __init__(self, *args, **kwargs):
        super(DoxyCompMem, self).__init__(*args, **kwargs)

    @classmethod
    def can_parse(cls, obj):
        return obj.kind == cls.kind

    def set_descriptions(self, parse_data):
        bd = description(getattr(parse_data, 'briefdescription', None))
        dd = description(getattr(parse_data, 'detaileddescription', None))
        self._data['brief_description'] = bd
        self._data['detailed_description'] = dd

    def set_parameters(self, data):
        vs = [ddc.value for ddc in data.detaileddescription.content_]
        pls = []
        for v in vs:
            if hasattr(v, 'parameterlist'):
                pls += v.parameterlist
        pis = []
        for pl in pls:
            pis += pl.parameteritem
        dpis = []
        for pi in pis:
            dpi = DoxyParameterItem(pi)
            dpi._parse()
            dpis.append(dpi)
        self._data['params'] = dpis


class DoxyCompound(DoxyCompMem):
    pass

class DoxyMember(DoxyCompMem):
    pass

class DoxyFunction(DoxyMember):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'function'

    def _parse(self):
        if self._parsed:
            return
        super(DoxyFunction, self)._parse()
        self.set_descriptions(self._parse_data)
        self.set_parameters(self._parse_data)
        if not self._data['params']:
            # If the params weren't set by a comment then just grab the names.
            self._data['params'] = []
            prms = self._parse_data.param
            for prm in prms:
                self._data['params'].append(DoxyParam(prm))

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])
    params = property(lambda self: self.data()['params'])

Base.mem_classes.append(DoxyFunction)


class DoxyParam(DoxyMember):

    __module__ = "gnuradio.utils.doxyxml"

    def _parse(self):
        if self._parsed:
            return
        super(DoxyParam, self)._parse()
        self.set_descriptions(self._parse_data)
        self._data['declname'] = self._parse_data.declname

    @property
    def description(self):
        descriptions = []
        if self.brief_description:
            descriptions.append(self.brief_description)
        if self.detailed_description:
            descriptions.append(self.detailed_description)
        return '\n\n'.join(descriptions)

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])
    name = property(lambda self: self.data()['declname'])

class DoxyParameterItem(DoxyMember):
    """A different representation of a parameter in Doxygen."""

    def _parse(self):
        if self._parsed:
            return
        super(DoxyParameterItem, self)._parse()
        names = []
        for nl in self._parse_data.parameternamelist:
            for pn in nl.parametername:
                names.append(description(pn))
        # Just take first name
        self._data['name'] = names[0]
        # Get description
        pd = description(self._parse_data.get_parameterdescription())
        self._data['description'] = pd

    description = property(lambda self: self.data()['description'])
    name = property(lambda self: self.data()['name'])


class DoxyClass(DoxyCompound):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'class'

    def _parse(self):
        if self._parsed:
            return
        super(DoxyClass, self)._parse()
        self.retrieve_data()
        if self._error:
            return
        self.set_descriptions(self._retrieved_data.compounddef)
        self.set_parameters(self._retrieved_data.compounddef)
        # Sectiondef.kind tells about whether private or public.
        # We just ignore this for now.
        self.process_memberdefs()

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])
    params = property(lambda self: self.data()['params'])

Base.mem_classes.append(DoxyClass)


class DoxyFile(DoxyCompound):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'file'

    def _parse(self):
        if self._parsed:
            return
        super(DoxyFile, self)._parse()
        self.retrieve_data()
        self.set_descriptions(self._retrieved_data.compounddef)
        if self._error:
            return
        self.process_memberdefs()

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])

Base.mem_classes.append(DoxyFile)


class DoxyNamespace(DoxyCompound):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'namespace'

    def _parse(self):
        if self._parsed:
            return
        super(DoxyNamespace, self)._parse()
        self.retrieve_data()
        self.set_descriptions(self._retrieved_data.compounddef)
        if self._error:
            return
        self.process_memberdefs()

Base.mem_classes.append(DoxyNamespace)


class DoxyGroup(DoxyCompound):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'group'

    def _parse(self):
        if self._parsed:
            return
        super(DoxyGroup, self)._parse()
        self.retrieve_data()
        if self._error:
            return
        cdef = self._retrieved_data.compounddef
        self._data['title'] = description(cdef.title)
        # Process inner groups
        grps = cdef.innergroup
        for grp in grps:
            converted = DoxyGroup.from_refid(grp.refid, top=self.top)
            self._members.append(converted)
        # Process inner classes
        klasses = cdef.innerclass
        for kls in klasses:
            converted = DoxyClass.from_refid(kls.refid, top=self.top)
            self._members.append(converted)
        # Process normal members
        self.process_memberdefs()

    title = property(lambda self: self.data()['title'])


Base.mem_classes.append(DoxyGroup)


class DoxyFriend(DoxyMember):

    __module__ = "gnuradio.utils.doxyxml"

    kind = 'friend'

Base.mem_classes.append(DoxyFriend)


class DoxyOther(Base):

    __module__ = "gnuradio.utils.doxyxml"

    kinds = set(['variable', 'struct', 'union', 'define', 'typedef', 'enum',
                 'dir', 'page', 'signal', 'slot', 'property'])

    @classmethod
    def can_parse(cls, obj):
        return obj.kind in cls.kinds

Base.mem_classes.append(DoxyOther)
