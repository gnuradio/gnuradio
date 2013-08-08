"""
This script regenerates the gnuradio.blocks sphinx source code.
"""

from gnuradio import analog
import sys
import os

doxyxml_location = os.path.abspath("../../../doxygen")
xml_location = os.path.abspath("../../../../build/docs/doxygen/xml/")
sys.path.append(doxyxml_location)

from doxyxml import DoxyIndex, DoxyClass, DoxyFriend, DoxyFunction, DoxyFile, DoxyGroup
from doxyxml import DoxyOther, base

class Block(object):
    """
    Checks if doxyxml produced objects correspond to a new style
    gnuradio block.
    """
    
    @classmethod
    def includes(cls, item):
        if not isinstance(item, DoxyClass):
            return False
        # Check for a parsing error.
        if item.error():
            return False
        is_a_block = item.has_member('make', DoxyFunction) and item.has_member('sptr', DoxyOther)
        return is_a_block

class Group(object):
    """
    Checks if doxyxml produced objects correspond to a group.
    """
    
    @classmethod
    def includes(cls, item):
        if not isinstance(item, DoxyGroup):
            return False
        # Check for a parsing error.
        if item.error():
            return False
        return True

def main():
    di = DoxyIndex(xml_location)
    blocks = di.in_category(Block)
    analog_blocks = []
    for block in blocks:
        if block.name().startswith("gr::analog::"):
            analog_blocks.append(block)

    all_grouped_blocks = []
    groups = di.in_category(Group)
    groupinfo = []
    for group in groups:
        contains_analog_blocks = False
        block_list = []
        members = group.members()
        for member in members:
            if member.name().startswith("gr::analog"):
                all_grouped_blocks.append(member)
                if not contains_analog_blocks:
                    contains_analog_blocks = True
                    groupinfo.append((group.name(), group.title, block_list))
                block_list.append(member)

    for block in analog_blocks:
        if block not in all_grouped_blocks:
            print("Didn't find block {0}".format(block.name()))

    blockindex = ["""gnuradio.analog
===============

.. automodule:: gnuradio.analog"""]
    for groupname, grouptitle, blocks in groupinfo:
        blockindex.append("")
        blockindex.append(grouptitle)
        blockindex.append('-'*len(grouptitle))
        blockindex.append("""
.. autosummary::
   :nosignatures:
""")
        for block in blocks:
            blockindex.append("   gnuradio.analog.{0}".format(
                block.name()[len("gr::analog::"):]))
        grouppage = []
        title = "gnuradio.analog: {0}".format(grouptitle)
        grouppage.append(title)
        grouppage.append('='*len(title))
        grouppage.append('')
        for block in blocks:
            shortname = block.name()[len("gr::analog::"):]
            grouppage.append(".. autoblock:: gnuradio.analog.{0}".format(shortname))
        text = '\n'.join(grouppage)
        f = open("{0}.rst".format(groupname), 'w')
        f.write(text)
        f.close()
    text = "\n".join(blockindex)
    f = open("index.rst", 'w')
    f.write(text)
    f.close()

if __name__ == '__main__':
    main()
