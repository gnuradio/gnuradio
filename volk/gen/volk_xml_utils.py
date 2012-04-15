#
# Copyright 2012 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#this would not be necessary if we just skipped the xml part and stored the data in python

import os
from xml.dom import minidom
import copy

gendir = os.path.dirname(__file__)
archs_xml = minidom.parse(os.path.join(gendir, 'archs.xml')).getElementsByTagName('arch')
machines_xml = minidom.parse(os.path.join(gendir, 'machines.xml')).getElementsByTagName('machine')
compilers_xml = minidom.parse(os.path.join(gendir, 'compilers.xml')).getElementsByTagName('compiler')

class arch_class:
    def __init__(self, arch_xml):
        self.name = arch_xml.attributes['name'].value
        self.type = arch_xml.attributes['type'].value
        for key in ('val', 'op', 'reg', 'shift', 'flag', 'environment', 'include', 'alignment'):
            try: setattr(self, key, arch_xml.getElementsByTagName(key)[0].firstChild.data)
            except: setattr(self, key, None)
        if self.alignment is None: self.alignment = 1

    def __repr__(self): return self.name

def get_archs():
    adict = dict([(a.name, a) for a in map(arch_class, archs_xml)])
    udict = dict()
    for name, arch in adict.iteritems():
        if arch.alignment == 1: continue
        uarch = copy.deepcopy(arch)
        uarch.name = arch.name + '_u'
        uarch.alignment = 1
        udict[uarch.name] = uarch
    adict.update(udict)
    return adict

volk_archs = get_archs()

class machine_class:
    def __init__(self, name, archlist):
        self.name = name
        self.archs = list()
        for arch_name in archlist:
            if not arch_name: continue
            self.archs.append(volk_archs[arch_name])
            if volk_archs.has_key(arch_name + '_u'):
                self.archs.append(volk_archs[arch_name + '_u'])

    def __repr__(self): return self.name

def yield_machines(name, archlist):
    for i, arch_name in enumerate(archlist):
        if '|' in arch_name:
            for arch_sub in arch_name.split('|'):
                for m in yield_machines(name, archlist[:i] + [arch_sub] + archlist[i+1:]):
                    yield m
            return
    yield machine_class(name, archlist)

def get_machines():
    mdict = dict()
    for machine_xml in machines_xml:
        name = machine_xml.attributes['name'].value
        archlist = machine_xml.getElementsByTagName('archs')[0].firstChild.data.split()
        for machine in yield_machines(name, archlist):
            mdict[machine.name] = machine
    return mdict

volk_machines = get_machines()

class compiler_class:
    def __init__(self, compiler_xml):
        self.name = compiler_xml.attributes['name'].value
        self.prefix = compiler_xml.getElementsByTagName('prefix')[0].firstChild.data
        self._remap = dict()
        for remap_xml in compiler_xml.getElementsByTagName('remap'):
            self._remap[remap_xml.attributes['name'].value] = remap_xml.firstChild.data

    def remap(self, option):
        if not self._remap.has_key(option): return option
        return self._remap[option]

    def __repr__(self): return self.name

def get_compilers():
    return dict([(c.name, c) for c in map(compiler_class, compilers_xml)])

volk_compilers = get_compilers()
