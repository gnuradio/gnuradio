import pmt

import runtime_swig as gr

class PythonTag(object):
    " Python container for tags "
    def __init__(self):
        self.offset = None
        self.key    = None
        self.value  = None
        self.srcid  = None

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


