#!/usr/bin/env python

import sys;
sys.path.append("../swig/");
sys.path.append("../swig/.libs/");
from gruel import pmt;

a = pmt.pmt_intern("a");
b = pmt.pmt_from_double(123765);
d1 = pmt.pmt_make_dict();
d2 = pmt.pmt_dict_add(d1, a, b);
pmt.pmt_print(d2);
print pmt.pmt_serialize_str(d2);

