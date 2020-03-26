#!/usr/bin/env python

"""
Generated Mon Feb  9 19:08:05 2009 by generateDS.py.
"""
from __future__ import absolute_import
from __future__ import unicode_literals

from xml.dom import minidom

import os
import sys
from . import compound

from . import indexsuper as supermod

class DoxygenTypeSub(supermod.DoxygenType):
    def __init__(self, version=None, compound=None):
        supermod.DoxygenType.__init__(self, version, compound)

    def find_compounds_and_members(self, details):
        """
        Returns a list of all compounds and their members which match details
        """

        results = []
        for compound in self.compound:
            members = compound.find_members(details)
            if members:
                results.append([compound, members])
            else:
                if details.match(compound):
                    results.append([compound, []])

        return results

supermod.DoxygenType.subclass = DoxygenTypeSub
# end class DoxygenTypeSub


class CompoundTypeSub(supermod.CompoundType):
    def __init__(self, kind=None, refid=None, name='', member=None):
        supermod.CompoundType.__init__(self, kind, refid, name, member)

    def find_members(self, details):
        """
        Returns a list of all members which match details
        """

        results = []

        for member in self.member:
            if details.match(member):
                results.append(member)

        return results

supermod.CompoundType.subclass = CompoundTypeSub
# end class CompoundTypeSub


class MemberTypeSub(supermod.MemberType):

    def __init__(self, kind=None, refid=None, name=''):
        supermod.MemberType.__init__(self, kind, refid, name)

supermod.MemberType.subclass = MemberTypeSub
# end class MemberTypeSub


def parse(inFilename):

    doc = minidom.parse(inFilename)
    rootNode = doc.documentElement
    rootObj = supermod.DoxygenType.factory()
    rootObj.build(rootNode)

    return rootObj

