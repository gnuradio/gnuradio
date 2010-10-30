"""
Classes providing more user-friendly interfaces to the doxygen xml
docs than the generated classes provide.
"""

import os

from gnuradio.utils.doxyxml.generated import index
from gnuradio.utils.doxyxml.base import Base
from gnuradio.utils.doxyxml.text import description

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
            # For files we want the contents to be accessible directly
            # from the parent rather than having to go through the file
            # object.  I'm not interested in the file object and will
            # not add it to the index.
            if self.get_cls(mem) == DoxyFile:
                if mem.name.endswith('.h'):
                    self._members += converted.members()
            else:
                self._members.append(converted)


def generate_swig_doc_i(self):
    """
    %feature("docstring") gr_make_align_on_samplenumbers_ss::align_state "
    Wraps the C++: gr_align_on_samplenumbers_ss::align_state";
    """
    pass


class DoxyCompMem(Base):


    kind = None

    def __init__(self, *args, **kwargs):
        super(DoxyCompMem, self).__init__(*args, **kwargs)
        #pd = self._parse_data
        #self.name = pd.name

    @classmethod
    def can_parse(cls, obj):
        return obj.kind == cls.kind

    def set_descriptions(self, parse_data):
        bd = description(getattr(parse_data, 'briefdescription', None))
        dd = description(getattr(parse_data, 'detaileddescription', None))
        self._data['brief_description'] = bd
        self._data['detailed_description'] = dd        

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

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])
    declname = property(lambda self: self.data()['declname'])

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
        # Sectiondef.kind tells about whether private or public.
        # We just ignore this for now.
        self.process_memberdefs()

    brief_description = property(lambda self: self.data()['brief_description'])
    detailed_description = property(lambda self: self.data()['detailed_description'])

Base.mem_classes.append(DoxyClass)
        

class DoxyFile(DoxyCompound):
    
    __module__ = "gnuradio.utils.doxyxml"

    kind = 'file'
    
    def _parse(self):
        if self._parsed:
            return
        super(DoxyFile, self)._parse()
        self.retrieve_data()
        if self._error:
            return
        self.process_memberdefs()
        

Base.mem_classes.append(DoxyFile)


class DoxyNamespace(DoxyCompound):
    
    __module__ = "gnuradio.utils.doxyxml"

    kind = 'namespace'
    
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

    kinds = set(['variable', 'struct', 'union', 'define', 'typedef', 'enum', 'dir', 'page'])

    @classmethod
    def can_parse(cls, obj):
        return obj.kind in cls.kinds
    
Base.mem_classes.append(DoxyOther)

