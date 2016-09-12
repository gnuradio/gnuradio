#
# Copyright 2010-2012 Free Software Foundation, Inc.
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
Creates the swig_doc.i SWIG interface file.
Execute using: python swig_doc.py xml_path outputfilename

The file instructs SWIG to transfer the doxygen comments into the
python docstrings.

"""

import sys, time

from doxyxml import DoxyIndex, DoxyClass, DoxyFriend, DoxyFunction, DoxyFile
from doxyxml import DoxyOther, base

def py_name(name):
    bits = name.split('_')
    return '_'.join(bits[1:])

def make_name(name):
    bits = name.split('_')
    return bits[0] + '_make_' + '_'.join(bits[1:])


class Block(object):
    """
    Checks if doxyxml produced objects correspond to a gnuradio block.
    """

    @classmethod
    def includes(cls, item):
        if not isinstance(item, DoxyClass):
            return False
        # Check for a parsing error.
        if item.error():
            return False
        friendname = make_name(item.name())
        is_a_block = item.has_member(friendname, DoxyFriend)
        # But now sometimes the make function isn't a friend so check again.
        if not is_a_block:
            is_a_block = di.has_member(friendname, DoxyFunction)
        return is_a_block

class Block2(object):
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
        is_a_block2 = item.has_member('make', DoxyFunction) and item.has_member('sptr', DoxyOther)
        return is_a_block2


def utoascii(text):
    """
    Convert unicode text into ascii and escape quotes.
    """
    if text is None:
        return ''
    out = text.encode('ascii', 'replace')
    out = out.replace('"', '\\"')
    return out


def combine_descriptions(obj):
    """
    Combines the brief and detailed descriptions of an object together.
    """
    description = []
    bd = obj.brief_description.strip()
    dd = obj.detailed_description.strip()
    if bd:
        description.append(bd)
    if dd:
        description.append(dd)
    return utoascii('\n\n'.join(description)).strip()

def format_params(parameteritems):
    output = ['Args:']
    template = '    {0} : {1}'
    for pi in parameteritems:
        output.append(template.format(pi.name, pi.description))
    return '\n'.join(output)

entry_templ = '%feature("docstring") {name} "{docstring}"'
def make_entry(obj, name=None, templ="{description}", description=None, params=[]):
    """
    Create a docstring entry for a swig interface file.

    obj - a doxyxml object from which documentation will be extracted.
    name - the name of the C object (defaults to obj.name())
    templ - an optional template for the docstring containing only one
            variable named 'description'.
    description - if this optional variable is set then it's value is
            used as the description instead of extracting it from obj.
    """
    if name is None:
        name=obj.name()
    if "operator " in name:
        return ''
    if description is None:
        description = combine_descriptions(obj)
    if params:
        description += '\n\n'
        description += utoascii(format_params(params))
    docstring = templ.format(description=description)
    if not docstring:
        return ''
    return entry_templ.format(
        name=name,
        docstring=docstring,
    )


def make_func_entry(func, name=None, description=None, params=None):
    """
    Create a function docstring entry for a swig interface file.

    func - a doxyxml object from which documentation will be extracted.
    name - the name of the C object (defaults to func.name())
    description - if this optional variable is set then it's value is
            used as the description instead of extracting it from func.
    params - a parameter list that overrides using func.params.
    """
    #if params is None:
    #    params = func.params
    #params = [prm.declname for prm in params]
    #if params:
    #    sig = "Params: (%s)" % ", ".join(params)
    #else:
    #    sig = "Params: (NONE)"
    #templ = "{description}\n\n" + sig
    #return make_entry(func, name=name, templ=utoascii(templ),
    #                  description=description)
    return make_entry(func, name=name, description=description, params=params)


def make_class_entry(klass, description=None, ignored_methods=[], params=None):
    """
    Create a class docstring for a swig interface file.
    """
    if params is None:
        params = klass.params
    output = []
    output.append(make_entry(klass, description=description, params=params))
    for func in klass.in_category(DoxyFunction):
        if func.name() not in ignored_methods:
            name = klass.name() + '::' + func.name()
            output.append(make_func_entry(func, name=name))
    return "\n\n".join(output)


def make_block_entry(di, block):
    """
    Create class and function docstrings of a gnuradio block for a
    swig interface file.
    """
    descriptions = []
    # Get the documentation associated with the class.
    class_desc = combine_descriptions(block)
    if class_desc:
        descriptions.append(class_desc)
    # Get the documentation associated with the make function
    make_func = di.get_member(make_name(block.name()), DoxyFunction)
    make_func_desc = combine_descriptions(make_func)
    if make_func_desc:
        descriptions.append(make_func_desc)
    # Get the documentation associated with the file
    try:
        block_file = di.get_member(block.name() + ".h", DoxyFile)
        file_desc = combine_descriptions(block_file)
        if file_desc:
            descriptions.append(file_desc)
    except base.Base.NoSuchMember:
        # Don't worry if we can't find a matching file.
        pass
    # And join them all together to make a super duper description.
    super_description = "\n\n".join(descriptions)
    # Associate the combined description with the class and
    # the make function.
    output = []
    output.append(make_class_entry(block, description=super_description))
    output.append(make_func_entry(make_func, description=super_description,
                                  params=block.params))
    return "\n\n".join(output)

def make_block2_entry(di, block):
    """
    Create class and function docstrings of a new style gnuradio block for a
    swig interface file.
    """
    descriptions = []
    # For new style blocks all the relevant documentation should be
    # associated with the 'make' method.
    class_description = combine_descriptions(block)
    make_func = block.get_member('make', DoxyFunction)
    make_description = combine_descriptions(make_func)
    description = class_description + "\n\nConstructor Specific Documentation:\n\n" + make_description
    # Associate the combined description with the class and
    # the make function.
    output = []
    output.append(make_class_entry(
        block, description=description,
        ignored_methods=['make'], params=make_func.params))
    makename = block.name() + '::make'
    output.append(make_func_entry(
        make_func, name=makename, description=description,
        params=make_func.params))
    return "\n\n".join(output)

def make_swig_interface_file(di, swigdocfilename, custom_output=None):

    output = ["""
/*
 * This file was automatically generated using swig_doc.py.
 *
 * Any changes to it will be lost next time it is regenerated.
 */
"""]

    if custom_output is not None:
        output.append(custom_output)

    # Create docstrings for the blocks.
    blocks = di.in_category(Block)
    blocks2 = di.in_category(Block2)

    make_funcs = set([])
    for block in blocks:
        try:
            make_func = di.get_member(make_name(block.name()), DoxyFunction)
            # Don't want to risk writing to output twice.
            if make_func.name() not in make_funcs:
                make_funcs.add(make_func.name())
                output.append(make_block_entry(di, block))
        except block.ParsingError:
            sys.stderr.write('Parsing error for block {0}\n'.format(block.name()))
            raise

    for block in blocks2:
        try:
            make_func = block.get_member('make', DoxyFunction)
            make_func_name = block.name() +'::make'
            # Don't want to risk writing to output twice.
            if make_func_name not in make_funcs:
                make_funcs.add(make_func_name)
                output.append(make_block2_entry(di, block))
        except block.ParsingError:
            sys.stderr.write('Parsing error for block {0}\n'.format(block.name()))
            raise

    # Create docstrings for functions
    # Don't include the make functions since they have already been dealt with.
    funcs = [f for f in di.in_category(DoxyFunction)
             if f.name() not in make_funcs and not f.name().startswith('std::')]
    for f in funcs:
        try:
            output.append(make_func_entry(f))
        except f.ParsingError:
            sys.stderr.write('Parsing error for function {0}\n'.format(f.name()))

    # Create docstrings for classes
    block_names = [block.name() for block in blocks]
    block_names += [block.name() for block in blocks2]
    klasses = [k for k in di.in_category(DoxyClass)
               if k.name() not in block_names and not k.name().startswith('std::')]
    for k in klasses:
        try:
            output.append(make_class_entry(k))
        except k.ParsingError:
            sys.stderr.write('Parsing error for class {0}\n'.format(k.name()))

    # Docstrings are not created for anything that is not a function or a class.
    # If this excludes anything important please add it here.

    output = "\n\n".join(output)

    swig_doc = file(swigdocfilename, 'w')
    swig_doc.write(output)
    swig_doc.close()

if __name__ == "__main__":
    # Parse command line options and set up doxyxml.
    err_msg = "Execute using: python swig_doc.py xml_path outputfilename"
    if len(sys.argv) != 3:
        raise StandardError(err_msg)
    xml_path = sys.argv[1]
    swigdocfilename = sys.argv[2]
    di = DoxyIndex(xml_path)

    # gnuradio.gr.msq_queue.insert_tail and delete_head create errors unless docstrings are defined!
    # This is presumably a bug in SWIG.
    #msg_q = di.get_member(u'gr_msg_queue', DoxyClass)
    #insert_tail = msg_q.get_member(u'insert_tail', DoxyFunction)
    #delete_head = msg_q.get_member(u'delete_head', DoxyFunction)
    output = []
    #output.append(make_func_entry(insert_tail, name='gr_py_msg_queue__insert_tail'))
    #output.append(make_func_entry(delete_head, name='gr_py_msg_queue__delete_head'))
    custom_output = "\n\n".join(output)

    # Generate the docstrings interface file.
    make_swig_interface_file(di, swigdocfilename, custom_output=custom_output)
