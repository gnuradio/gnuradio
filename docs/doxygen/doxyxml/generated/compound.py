#!/usr/bin/env python

"""
Generated Mon Feb  9 19:08:05 2009 by generateDS.py.
"""


from xml.dom import minidom
from xml.dom import Node

import sys

from . import compoundsuper as supermod
from .compoundsuper import MixedContainer


class DoxygenTypeSub(supermod.DoxygenType):
    def __init__(self, version=None, compounddef=None):
        supermod.DoxygenType.__init__(self, version, compounddef)

    def find(self, details):

        return self.compounddef.find(details)


supermod.DoxygenType.subclass = DoxygenTypeSub
# end class DoxygenTypeSub


class compounddefTypeSub(supermod.compounddefType):
    def __init__(self, kind=None, prot=None, id=None, compoundname='', title='', basecompoundref=None, derivedcompoundref=None, includes=None, includedby=None, incdepgraph=None, invincdepgraph=None, innerdir=None, innerfile=None, innerclass=None, innernamespace=None, innerpage=None, innergroup=None, templateparamlist=None, sectiondef=None, briefdescription=None, detaileddescription=None, inheritancegraph=None, collaborationgraph=None, programlisting=None, location=None, listofallmembers=None):
        supermod.compounddefType.__init__(self, kind, prot, id, compoundname, title, basecompoundref, derivedcompoundref, includes, includedby, incdepgraph, invincdepgraph, innerdir, innerfile, innerclass,
                                          innernamespace, innerpage, innergroup, templateparamlist, sectiondef, briefdescription, detaileddescription, inheritancegraph, collaborationgraph, programlisting, location, listofallmembers)

    def find(self, details):

        if self.id == details.refid:
            return self

        for sectiondef in self.sectiondef:
            result = sectiondef.find(details)
            if result:
                return result


supermod.compounddefType.subclass = compounddefTypeSub
# end class compounddefTypeSub


class listofallmembersTypeSub(supermod.listofallmembersType):
    def __init__(self, member=None):
        supermod.listofallmembersType.__init__(self, member)


supermod.listofallmembersType.subclass = listofallmembersTypeSub
# end class listofallmembersTypeSub


class memberRefTypeSub(supermod.memberRefType):
    def __init__(self, virt=None, prot=None, refid=None, ambiguityscope=None, scope='', name=''):
        supermod.memberRefType.__init__(
            self, virt, prot, refid, ambiguityscope, scope, name)


supermod.memberRefType.subclass = memberRefTypeSub
# end class memberRefTypeSub


class compoundRefTypeSub(supermod.compoundRefType):
    def __init__(self, virt=None, prot=None, refid=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.compoundRefType.__init__(self, mixedclass_, content_)


supermod.compoundRefType.subclass = compoundRefTypeSub
# end class compoundRefTypeSub


class reimplementTypeSub(supermod.reimplementType):
    def __init__(self, refid=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.reimplementType.__init__(self, mixedclass_, content_)


supermod.reimplementType.subclass = reimplementTypeSub
# end class reimplementTypeSub


class incTypeSub(supermod.incType):
    def __init__(self, local=None, refid=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.incType.__init__(self, mixedclass_, content_)


supermod.incType.subclass = incTypeSub
# end class incTypeSub


class refTypeSub(supermod.refType):
    def __init__(self, prot=None, refid=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.refType.__init__(self, mixedclass_, content_)


supermod.refType.subclass = refTypeSub
# end class refTypeSub


class refTextTypeSub(supermod.refTextType):
    def __init__(self, refid=None, kindref=None, external=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.refTextType.__init__(self, mixedclass_, content_)


supermod.refTextType.subclass = refTextTypeSub
# end class refTextTypeSub


class sectiondefTypeSub(supermod.sectiondefType):

    def __init__(self, kind=None, header='', description=None, memberdef=None):
        supermod.sectiondefType.__init__(
            self, kind, header, description, memberdef)

    def find(self, details):

        for memberdef in self.memberdef:
            if memberdef.id == details.refid:
                return memberdef

        return None


supermod.sectiondefType.subclass = sectiondefTypeSub
# end class sectiondefTypeSub


class memberdefTypeSub(supermod.memberdefType):
    def __init__(self, initonly=None, kind=None, volatile=None, const=None, raise_=None, virt=None, readable=None, prot=None, explicit=None, new=None, final=None, writable=None, add=None, static=None, remove=None, sealed=None, mutable=None, gettable=None, inline=None, settable=None, id=None, templateparamlist=None, type_=None, definition='', argsstring='', name='', read='', write='', bitfield='', reimplements=None, reimplementedby=None, param=None, enumvalue=None, initializer=None, exceptions=None, briefdescription=None, detaileddescription=None, inbodydescription=None, location=None, references=None, referencedby=None):
        supermod.memberdefType.__init__(self, initonly, kind, volatile, const, raise_, virt, readable, prot, explicit, new, final, writable, add, static, remove, sealed, mutable, gettable, inline, settable, id, templateparamlist, type_,
                                        definition, argsstring, name, read, write, bitfield, reimplements, reimplementedby, param, enumvalue, initializer, exceptions, briefdescription, detaileddescription, inbodydescription, location, references, referencedby)


supermod.memberdefType.subclass = memberdefTypeSub
# end class memberdefTypeSub


class descriptionTypeSub(supermod.descriptionType):
    def __init__(self, title='', para=None, sect1=None, internal=None, mixedclass_=None, content_=None):
        supermod.descriptionType.__init__(self, mixedclass_, content_)


supermod.descriptionType.subclass = descriptionTypeSub
# end class descriptionTypeSub


class enumvalueTypeSub(supermod.enumvalueType):
    def __init__(self, prot=None, id=None, name='', initializer=None, briefdescription=None, detaileddescription=None, mixedclass_=None, content_=None):
        supermod.enumvalueType.__init__(self, mixedclass_, content_)


supermod.enumvalueType.subclass = enumvalueTypeSub
# end class enumvalueTypeSub


class templateparamlistTypeSub(supermod.templateparamlistType):
    def __init__(self, param=None):
        supermod.templateparamlistType.__init__(self, param)


supermod.templateparamlistType.subclass = templateparamlistTypeSub
# end class templateparamlistTypeSub


class paramTypeSub(supermod.paramType):
    def __init__(self, type_=None, declname='', defname='', array='', defval=None, briefdescription=None):
        supermod.paramType.__init__(
            self, type_, declname, defname, array, defval, briefdescription)


supermod.paramType.subclass = paramTypeSub
# end class paramTypeSub


class linkedTextTypeSub(supermod.linkedTextType):
    def __init__(self, ref=None, mixedclass_=None, content_=None):
        supermod.linkedTextType.__init__(self, mixedclass_, content_)


supermod.linkedTextType.subclass = linkedTextTypeSub
# end class linkedTextTypeSub


class graphTypeSub(supermod.graphType):
    def __init__(self, node=None):
        supermod.graphType.__init__(self, node)


supermod.graphType.subclass = graphTypeSub
# end class graphTypeSub


class nodeTypeSub(supermod.nodeType):
    def __init__(self, id=None, label='', link=None, childnode=None):
        supermod.nodeType.__init__(self, id, label, link, childnode)


supermod.nodeType.subclass = nodeTypeSub
# end class nodeTypeSub


class childnodeTypeSub(supermod.childnodeType):
    def __init__(self, relation=None, refid=None, edgelabel=None):
        supermod.childnodeType.__init__(self, relation, refid, edgelabel)


supermod.childnodeType.subclass = childnodeTypeSub
# end class childnodeTypeSub


class linkTypeSub(supermod.linkType):
    def __init__(self, refid=None, external=None, valueOf_=''):
        supermod.linkType.__init__(self, refid, external)


supermod.linkType.subclass = linkTypeSub
# end class linkTypeSub


class listingTypeSub(supermod.listingType):
    def __init__(self, codeline=None):
        supermod.listingType.__init__(self, codeline)


supermod.listingType.subclass = listingTypeSub
# end class listingTypeSub


class codelineTypeSub(supermod.codelineType):
    def __init__(self, external=None, lineno=None, refkind=None, refid=None, highlight=None):
        supermod.codelineType.__init__(
            self, external, lineno, refkind, refid, highlight)


supermod.codelineType.subclass = codelineTypeSub
# end class codelineTypeSub


class highlightTypeSub(supermod.highlightType):
    def __init__(self, class_=None, sp=None, ref=None, mixedclass_=None, content_=None):
        supermod.highlightType.__init__(self, mixedclass_, content_)


supermod.highlightType.subclass = highlightTypeSub
# end class highlightTypeSub


class referenceTypeSub(supermod.referenceType):
    def __init__(self, endline=None, startline=None, refid=None, compoundref=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.referenceType.__init__(self, mixedclass_, content_)


supermod.referenceType.subclass = referenceTypeSub
# end class referenceTypeSub


class locationTypeSub(supermod.locationType):
    def __init__(self, bodystart=None, line=None, bodyend=None, bodyfile=None, file=None, valueOf_=''):
        supermod.locationType.__init__(
            self, bodystart, line, bodyend, bodyfile, file)


supermod.locationType.subclass = locationTypeSub
# end class locationTypeSub


class docSect1TypeSub(supermod.docSect1Type):
    def __init__(self, id=None, title='', para=None, sect2=None, internal=None, mixedclass_=None, content_=None):
        supermod.docSect1Type.__init__(self, mixedclass_, content_)


supermod.docSect1Type.subclass = docSect1TypeSub
# end class docSect1TypeSub


class docSect2TypeSub(supermod.docSect2Type):
    def __init__(self, id=None, title='', para=None, sect3=None, internal=None, mixedclass_=None, content_=None):
        supermod.docSect2Type.__init__(self, mixedclass_, content_)


supermod.docSect2Type.subclass = docSect2TypeSub
# end class docSect2TypeSub


class docSect3TypeSub(supermod.docSect3Type):
    def __init__(self, id=None, title='', para=None, sect4=None, internal=None, mixedclass_=None, content_=None):
        supermod.docSect3Type.__init__(self, mixedclass_, content_)


supermod.docSect3Type.subclass = docSect3TypeSub
# end class docSect3TypeSub


class docSect4TypeSub(supermod.docSect4Type):
    def __init__(self, id=None, title='', para=None, internal=None, mixedclass_=None, content_=None):
        supermod.docSect4Type.__init__(self, mixedclass_, content_)


supermod.docSect4Type.subclass = docSect4TypeSub
# end class docSect4TypeSub


class docInternalTypeSub(supermod.docInternalType):
    def __init__(self, para=None, sect1=None, mixedclass_=None, content_=None):
        supermod.docInternalType.__init__(self, mixedclass_, content_)


supermod.docInternalType.subclass = docInternalTypeSub
# end class docInternalTypeSub


class docInternalS1TypeSub(supermod.docInternalS1Type):
    def __init__(self, para=None, sect2=None, mixedclass_=None, content_=None):
        supermod.docInternalS1Type.__init__(self, mixedclass_, content_)


supermod.docInternalS1Type.subclass = docInternalS1TypeSub
# end class docInternalS1TypeSub


class docInternalS2TypeSub(supermod.docInternalS2Type):
    def __init__(self, para=None, sect3=None, mixedclass_=None, content_=None):
        supermod.docInternalS2Type.__init__(self, mixedclass_, content_)


supermod.docInternalS2Type.subclass = docInternalS2TypeSub
# end class docInternalS2TypeSub


class docInternalS3TypeSub(supermod.docInternalS3Type):
    def __init__(self, para=None, sect3=None, mixedclass_=None, content_=None):
        supermod.docInternalS3Type.__init__(self, mixedclass_, content_)


supermod.docInternalS3Type.subclass = docInternalS3TypeSub
# end class docInternalS3TypeSub


class docInternalS4TypeSub(supermod.docInternalS4Type):
    def __init__(self, para=None, mixedclass_=None, content_=None):
        supermod.docInternalS4Type.__init__(self, mixedclass_, content_)


supermod.docInternalS4Type.subclass = docInternalS4TypeSub
# end class docInternalS4TypeSub


class docURLLinkSub(supermod.docURLLink):
    def __init__(self, url=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docURLLink.__init__(self, mixedclass_, content_)


supermod.docURLLink.subclass = docURLLinkSub
# end class docURLLinkSub


class docAnchorTypeSub(supermod.docAnchorType):
    def __init__(self, id=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docAnchorType.__init__(self, mixedclass_, content_)


supermod.docAnchorType.subclass = docAnchorTypeSub
# end class docAnchorTypeSub


class docFormulaTypeSub(supermod.docFormulaType):
    def __init__(self, id=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docFormulaType.__init__(self, mixedclass_, content_)


supermod.docFormulaType.subclass = docFormulaTypeSub
# end class docFormulaTypeSub


class docIndexEntryTypeSub(supermod.docIndexEntryType):
    def __init__(self, primaryie='', secondaryie=''):
        supermod.docIndexEntryType.__init__(self, primaryie, secondaryie)


supermod.docIndexEntryType.subclass = docIndexEntryTypeSub
# end class docIndexEntryTypeSub


class docListTypeSub(supermod.docListType):
    def __init__(self, listitem=None):
        supermod.docListType.__init__(self, listitem)


supermod.docListType.subclass = docListTypeSub
# end class docListTypeSub


class docListItemTypeSub(supermod.docListItemType):
    def __init__(self, para=None):
        supermod.docListItemType.__init__(self, para)


supermod.docListItemType.subclass = docListItemTypeSub
# end class docListItemTypeSub


class docSimpleSectTypeSub(supermod.docSimpleSectType):
    def __init__(self, kind=None, title=None, para=None):
        supermod.docSimpleSectType.__init__(self, kind, title, para)


supermod.docSimpleSectType.subclass = docSimpleSectTypeSub
# end class docSimpleSectTypeSub


class docVarListEntryTypeSub(supermod.docVarListEntryType):
    def __init__(self, term=None):
        supermod.docVarListEntryType.__init__(self, term)


supermod.docVarListEntryType.subclass = docVarListEntryTypeSub
# end class docVarListEntryTypeSub


class docRefTextTypeSub(supermod.docRefTextType):
    def __init__(self, refid=None, kindref=None, external=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docRefTextType.__init__(self, mixedclass_, content_)


supermod.docRefTextType.subclass = docRefTextTypeSub
# end class docRefTextTypeSub


class docTableTypeSub(supermod.docTableType):
    def __init__(self, rows=None, cols=None, row=None, caption=None):
        supermod.docTableType.__init__(self, rows, cols, row, caption)


supermod.docTableType.subclass = docTableTypeSub
# end class docTableTypeSub


class docRowTypeSub(supermod.docRowType):
    def __init__(self, entry=None):
        supermod.docRowType.__init__(self, entry)


supermod.docRowType.subclass = docRowTypeSub
# end class docRowTypeSub


class docEntryTypeSub(supermod.docEntryType):
    def __init__(self, thead=None, para=None):
        supermod.docEntryType.__init__(self, thead, para)


supermod.docEntryType.subclass = docEntryTypeSub
# end class docEntryTypeSub


class docHeadingTypeSub(supermod.docHeadingType):
    def __init__(self, level=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docHeadingType.__init__(self, mixedclass_, content_)


supermod.docHeadingType.subclass = docHeadingTypeSub
# end class docHeadingTypeSub


class docImageTypeSub(supermod.docImageType):
    def __init__(self, width=None, type_=None, name=None, height=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docImageType.__init__(self, mixedclass_, content_)


supermod.docImageType.subclass = docImageTypeSub
# end class docImageTypeSub


class docDotFileTypeSub(supermod.docDotFileType):
    def __init__(self, name=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docDotFileType.__init__(self, mixedclass_, content_)


supermod.docDotFileType.subclass = docDotFileTypeSub
# end class docDotFileTypeSub


class docTocItemTypeSub(supermod.docTocItemType):
    def __init__(self, id=None, valueOf_='', mixedclass_=None, content_=None):
        supermod.docTocItemType.__init__(self, mixedclass_, content_)


supermod.docTocItemType.subclass = docTocItemTypeSub
# end class docTocItemTypeSub


class docTocListTypeSub(supermod.docTocListType):
    def __init__(self, tocitem=None):
        supermod.docTocListType.__init__(self, tocitem)


supermod.docTocListType.subclass = docTocListTypeSub
# end class docTocListTypeSub


class docLanguageTypeSub(supermod.docLanguageType):
    def __init__(self, langid=None, para=None):
        supermod.docLanguageType.__init__(self, langid, para)


supermod.docLanguageType.subclass = docLanguageTypeSub
# end class docLanguageTypeSub


class docParamListTypeSub(supermod.docParamListType):
    def __init__(self, kind=None, parameteritem=None):
        supermod.docParamListType.__init__(self, kind, parameteritem)


supermod.docParamListType.subclass = docParamListTypeSub
# end class docParamListTypeSub


class docParamListItemSub(supermod.docParamListItem):
    def __init__(self, parameternamelist=None, parameterdescription=None):
        supermod.docParamListItem.__init__(
            self, parameternamelist, parameterdescription)


supermod.docParamListItem.subclass = docParamListItemSub
# end class docParamListItemSub


class docParamNameListSub(supermod.docParamNameList):
    def __init__(self, parametername=None):
        supermod.docParamNameList.__init__(self, parametername)


supermod.docParamNameList.subclass = docParamNameListSub
# end class docParamNameListSub


class docParamNameSub(supermod.docParamName):
    def __init__(self, direction=None, ref=None, mixedclass_=None, content_=None):
        supermod.docParamName.__init__(self, mixedclass_, content_)


supermod.docParamName.subclass = docParamNameSub
# end class docParamNameSub


class docXRefSectTypeSub(supermod.docXRefSectType):
    def __init__(self, id=None, xreftitle=None, xrefdescription=None):
        supermod.docXRefSectType.__init__(self, id, xreftitle, xrefdescription)


supermod.docXRefSectType.subclass = docXRefSectTypeSub
# end class docXRefSectTypeSub


class docCopyTypeSub(supermod.docCopyType):
    def __init__(self, link=None, para=None, sect1=None, internal=None):
        supermod.docCopyType.__init__(self, link, para, sect1, internal)


supermod.docCopyType.subclass = docCopyTypeSub
# end class docCopyTypeSub


class docCharTypeSub(supermod.docCharType):
    def __init__(self, char=None, valueOf_=''):
        supermod.docCharType.__init__(self, char)


supermod.docCharType.subclass = docCharTypeSub
# end class docCharTypeSub


class docParaTypeSub(supermod.docParaType):
    def __init__(self, char=None, valueOf_=''):
        supermod.docParaType.__init__(self, char)

        self.parameterlist = []
        self.simplesects = []
        self.content = []

    def buildChildren(self, child_, nodeName_):
        supermod.docParaType.buildChildren(self, child_, nodeName_)

        if child_.nodeType == Node.TEXT_NODE:
            obj_ = self.mixedclass_(MixedContainer.CategoryText,
                                    MixedContainer.TypeNone, '', child_.nodeValue)
            self.content.append(obj_)
        elif child_.nodeType == Node.ELEMENT_NODE and \
                nodeName_ == "ref":
            obj_ = supermod.docRefTextType.factory()
            obj_.build(child_)
            self.content.append(obj_)
        elif child_.nodeType == Node.ELEMENT_NODE and \
                nodeName_ == 'parameterlist':
            obj_ = supermod.docParamListType.factory()
            obj_.build(child_)
            self.parameterlist.append(obj_)
        elif child_.nodeType == Node.ELEMENT_NODE and \
                nodeName_ == 'simplesect':
            obj_ = supermod.docSimpleSectType.factory()
            obj_.build(child_)
            self.simplesects.append(obj_)


supermod.docParaType.subclass = docParaTypeSub
# end class docParaTypeSub


def parse(inFilename):
    doc = minidom.parse(inFilename)
    rootNode = doc.documentElement
    rootObj = supermod.DoxygenType.factory()
    rootObj.build(rootNode)
    return rootObj
