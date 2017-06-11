import pmt

import runtime_swig as gr

class PythonTag(object):
    " Python container for tags "
    def __init__(self):
        self.offset = None
        self.key    = None
        self.value  = None
        self.srcid  = False

def tag_to_python(tag):
    """ Convert a stream tag to a Python-readable object """
    newtag = PythonTag()
    newtag.offset = tag.offset
    newtag.key = pmt.to_python(tag.key)
    newtag.value = pmt.to_python(tag.value)
    newtag.srcid = pmt.to_python(tag.srcid)
    return newtag

def tag_to_pmt(tag):
    """ Convert a Python-readable object to a stream tag """
    newtag = gr.tag_t()
    newtag.offset = tag.offset
    newtag.key = pmt.to_python(tag.key)
    newtag.value = pmt.from_python(tag.value)
    newtag.srcid = pmt.from_python(tag.srcid)
    return newtag

def python_to_tag(tag_struct):
    """
    Convert a Python list/tuple/dictionary to a stream tag.

    When using a list or tuple format, this function expects the format:
      tag_struct[0] --> tag's offset (as an integer)
      tag_struct[1] --> tag's key (as a PMT)
      tag_struct[2] --> tag's value (as a PMT)
      tag_struct[3] --> tag's srcid (as a PMT)

    When using a dictionary, we specify the dictionary keys using:
      tag_struct['offset'] --> tag's offset (as an integer)
      tag_struct['key'] --> tag's key (as a PMT)
      tag_struct['value'] --> tag's value (as a PMT)
      tag_struct['srcid'] --> tag's srcid (as a PMT)

    If the function can take the Python object and successfully
    construct a tag, it will return the tag. Otherwise, it will return
    None.
    """
    good = False
    tag = gr.tag_t()
    if(type(tag_struct) == dict):
        if(tag_struct.has_key('offset')):
            if(isinstance(tag_struct['offset'], (int,long))):
                tag.offset = tag_struct['offset']
                good = True

        if(tag_struct.has_key('key')):
            if(isinstance(tag_struct['key'], pmt.swig_int_ptr)):
                tag.key = tag_struct['key']
                good = True

        if(tag_struct.has_key('value')):
            if(isinstance(tag_struct['value'], pmt.swig_int_ptr)):
                tag.value = tag_struct['value']
                good = True

        if(tag_struct.has_key('srcid')):
            if(isinstance(tag_struct['srcid'], pmt.swig_int_ptr)):
                tag.srcid = tag_struct['srcid']
                good = True

    elif(type(tag_struct) == list or type(tag_struct) == tuple):
        if(len(tag_struct) == 4):
            if(isinstance(tag_struct[0], (int,long))):
                tag.offset = tag_struct[0]
                good = True

            if(isinstance(tag_struct[1], pmt.swig_int_ptr)):
                tag.key = tag_struct[1]
                good = True

            if(isinstance(tag_struct[2], pmt.swig_int_ptr)):
                tag.value = tag_struct[2]
                good = True

            if(isinstance(tag_struct[3], pmt.swig_int_ptr)):
                tag.srcid = tag_struct[3]
                good = True

        elif(len(tag_struct) == 3):
            if(isinstance(tag_struct[0], (int,long))):
                tag.offset = tag_struct[0]
                good = True

            if(isinstance(tag_struct[1], pmt.swig_int_ptr)):
                tag.key = tag_struct[1]
                good = True

            if(isinstance(tag_struct[2], pmt.swig_int_ptr)):
                tag.value = tag_struct[2]
                good = True

            tag.srcid = pmt.PMT_F

    if(good):
        return tag
    else:
        return None

def tag_t_offset_compare_key():
    """
    Convert a tag_t_offset_compare function into a key=function
    This method is modeled after functools.cmp_to_key(_func_).
    It can be used by functions that accept a key function, such as
    sorted(), min(), max(), etc. to compare tags by their offsets,
    e.g., sorted(tag_list, key=gr.tag_t_offset_compare_key()).
    """
    class K(object):
        def __init__(self, obj, *args):
            self.obj = obj
        def __lt__(self, other):
            # x.offset < y.offset
            return gr.tag_t_offset_compare(self.obj, other.obj)
        def __gt__(self, other):
            # y.offset < x.offset
            return gr.tag_t_offset_compare(other.obj, self.obj)
        def __eq__(self, other):
            # not (x.offset < y.offset) and not (y.offset < x.offset)
            return not gr.tag_t_offset_compare(self.obj, other.obj) and \
                   not gr.tag_t_offset_compare(other.obj, self.obj)
        def __le__(self, other):
            # not (y.offset < x.offset)
            return not gr.tag_t_offset_compare(other.obj, self.obj)
        def __ge__(self, other):
            # not (x.offset < y.offset)
            return not gr.tag_t_offset_compare(self.obj, other.obj)
        def __ne__(self, other):
            # (x.offset < y.offset) or (y.offset < x.offset)
            return gr.tag_t_offset_compare(self.obj, other.obj) or \
                   gr.tag_t_offset_compare(other.obj, self.obj)
    return K
