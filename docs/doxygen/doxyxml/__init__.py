#
# Copyright 2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
Python interface to contents of doxygen xml documentation.

Example use:
See the contents of the example folder for the C++ and
doxygen-generated xml used in this example.

>>> # Parse the doxygen docs.
>>> import os
>>> this_dir = os.path.dirname(globals()['__file__'])
>>> xml_path = this_dir + "/example/xml/"
>>> di = DoxyIndex(xml_path)

Get a list of all top-level objects.

>>> print([mem.name() for mem in di.members()])
[u'Aadvark', u'aadvarky_enough', u'main']

Get all functions.

>>> print([mem.name() for mem in di.in_category(DoxyFunction)])
[u'aadvarky_enough', u'main']

Check if an object is present.

>>> di.has_member(u'Aadvark')
True
>>> di.has_member(u'Fish')
False

Get an item by name and check its properties.

>>> aad = di.get_member(u'Aadvark')
>>> print(aad.brief_description)
Models the mammal Aadvark.
>>> print(aad.detailed_description)
Sadly the model is incomplete and cannot capture all aspects of an aadvark yet.
<BLANKLINE>
This line is uninformative and is only to test line breaks in the comments.
>>> [mem.name() for mem in aad.members()]
[u'aadvarkness', u'print', u'Aadvark', u'get_aadvarkness']
>>> aad.get_member(u'print').brief_description
u'Outputs the vital aadvark statistics.'

"""

from .doxyindex import DoxyIndex, DoxyFunction, DoxyParam, DoxyClass, DoxyFile, DoxyNamespace, DoxyGroup, DoxyFriend, DoxyOther

def _test():
    import os
    this_dir = os.path.dirname(globals()['__file__'])
    xml_path = this_dir + "/example/xml/"
    di = DoxyIndex(xml_path)
    # Get the Aadvark class
    aad = di.get_member('Aadvark')
    aad.brief_description
    import doctest
    return doctest.testmod()

if __name__ == "__main__":
    _test()

