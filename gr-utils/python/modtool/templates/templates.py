#
# Copyright 2013-2014, 2018 Free Software Foundation, Inc.
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
''' All the templates for skeleton files (needed by ModToolAdd) '''

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

from datetime import datetime

Templates = {}

# Default licence
Templates['defaultlicense'] = '''
Copyright %d ${copyrightholder}.

This is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street,
Boston, MA 02110-1301, USA.
''' % datetime.now().year

Templates['grlicense'] = '''
Copyright {0} Free Software Foundation, Inc.

This file is part of GNU Radio

GNU Radio is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GNU Radio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Radio; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street,
Boston, MA 02110-1301, USA.
'''.format(datetime.now().year)

# Header file of a sync/decimator/interpolator block
Templates['block_impl_h'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}
#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H
#define INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H

#include <${include_dir_prefix}/${blockname}.h>

namespace gr {
  namespace ${modname} {

    class ${blockname}_impl : public ${blockname}
    {
     private:
      // Nothing to declare in this block.

% if blocktype == 'tagged_stream':
     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

% endif
     public:
      ${blockname}_impl(${strip_default_values(arglist)});
      ~${blockname}_impl();

      // Where all the action really happens
% if blocktype == 'general':
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

% elif blocktype == 'tagged_stream':
      int work(
              int noutput_items,
              gr_vector_int &ninput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
% elif blocktype == 'hier':
% else:
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
% endif
    };

  } // namespace ${modname}
} // namespace gr

#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H */

'''

# C++ file of a GR block
Templates['block_impl_cpp'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
% if blocktype == 'noblock':
#include <${include_dir_prefix}/${blockname}.h>
% else:
#include "${blockname}_impl.h"
% endif

namespace gr {
  namespace ${modname} {

% if blocktype == 'noblock':
    ${blockname}::${blockname}(${strip_default_values(arglist)})
    {
    }

    ${blockname}::~${blockname}()
    {
    }
% else:
    ${blockname}::sptr
    ${blockname}::make(${strip_default_values(arglist)})
    {
      return gnuradio::get_initial_sptr
        (new ${blockname}_impl(${strip_arg_types(arglist)}));
    }

<%
    if blocktype == 'decimator':
        decimation = ', <+decimation+>'
    elif blocktype == 'interpolator':
        decimation = ', <+interpolation+>'
    elif blocktype == 'tagged_stream':
        decimation = ', <+len_tag_key+>'
    else:
        decimation = ''
    endif
    if blocktype == 'source':
        inputsig = '0, 0, 0'
    else:
        inputsig = '<+MIN_IN+>, <+MAX_IN+>, sizeof(<+ITYPE+>)'
    endif
    if blocktype == 'sink':
        outputsig = '0, 0, 0'
    else:
        outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, sizeof(<+OTYPE+>)'
    endif
%>
    /*
     * The private constructor
     */
    ${blockname}_impl::${blockname}_impl(${strip_default_values(arglist)})
      : gr::${grblocktype}("${blockname}",
              gr::io_signature::make(${inputsig}),
              gr::io_signature::make(${outputsig})${decimation})
  % if blocktype == 'hier':
    {
      connect(self(), 0, d_firstblock, 0);
      // connect other blocks
      connect(d_lastblock, 0, self(), 0);
    }
  % else:
    {}
  % endif

    /*
     * Our virtual destructor.
     */
    ${blockname}_impl::~${blockname}_impl()
    {
    }

  % if blocktype == 'general':
    void
    ${blockname}_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    ${blockname}_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  % elif blocktype == 'tagged_stream':
    int
    ${blockname}_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = /* <+set this+> */;
      return noutput_items ;
    }

    int
    ${blockname}_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  % elif blocktype == 'hier':
  % else:
    int
    ${blockname}_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
    % if blocktype != 'source':
      const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
    % endif
    % if blocktype != 'sink':
      <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];
    % endif

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  % endif
% endif

  } /* namespace ${modname} */
} /* namespace gr */

'''

# Block definition header file (for include/)
Templates['block_def_h'] = r'''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}

#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_H
#define INCLUDED_${modname.upper()}_${blockname.upper()}_H

#include <${include_dir_prefix}/api.h>
% if blocktype != 'noblock':
#include <gnuradio/${grblocktype}.h>
% endif

namespace gr {
  namespace ${modname} {

% if blocktype == 'noblock':
    /*!
     * \brief <+description+>
     *
     */
    class ${modname.upper()}_API ${blockname}
    {
    public:
      ${blockname}(${arglist});
      ~${blockname}();
    private:
    };
% else:
    /*!
     * \brief <+description of block+>
     * \ingroup ${modname}
     *
     */
    class ${modname.upper()}_API ${blockname} : virtual public gr::${grblocktype}
    {
     public:
      typedef boost::shared_ptr<${blockname}> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ${modname}::${blockname}.
       *
       * To avoid accidental use of raw pointers, ${modname}::${blockname}'s
       * constructor is in a private implementation
       * class. ${modname}::${blockname}::make is the public interface for
       * creating new instances.
       */
      static sptr make(${arglist});
    };
% endif

  } // namespace ${modname}
} // namespace gr

#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_H */

'''

# Python block
Templates['block_python'] = '''#!/usr/bin/env python
# -*- coding: utf-8 -*-
${str_to_python_comment(license)}
#\
<%
    if blocktype == 'noblock':
        return
    if blocktype in ('sync', 'sink', 'source'):
        parenttype = 'gr.sync_block'
    else:
        parenttype = {
            'hier': 'gr.hier_block2',
            'interpolator': 'gr.interp_block',
            'decimator': 'gr.decim_block',
            'general': 'gr.basic_block'
        }[blocktype]
%>
% if blocktype != 'hier':

import numpy\
<%
    if blocktype == 'source':
        inputsig = 'None'
    else:
        inputsig = '[<+numpy.float32+>]'
    if blocktype == 'sink':
        outputsig = 'None'
    else:
        outputsig = '[<+numpy.float32+>]'
%>
% else:
<%
    if blocktype == 'source':
        inputsig = '0, 0, 0'
    else:
        inputsig = '<+MIN_IN+>, <+MAX_IN+>, gr.sizeof_<+ITYPE+>'
    if blocktype == 'sink':
        outputsig = '0, 0, 0'
    else:
        outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, gr.sizeof_<+OTYPE+>'
%>
% endif
<%
    if blocktype == 'interpolator':
        deciminterp = ', <+interpolation+>'
    elif blocktype == 'decimator':
        deciminterp = ', <+decimation+>'
    else:
        deciminterp = ''
    if arglist == '':
        arglistsep = ''
    else:
        arglistsep = ', '
%>from gnuradio import gr

class ${blockname}(${parenttype}):
    """
    docstring for block ${blockname}
    """
    def __init__(self${arglistsep}${arglist}):
        ${parenttype}.__init__(self,
% if blocktype == 'hier':
            "${blockname}",
            "gr.io_signature(${inputsig})",  # Input signature
            "gr.io_signature(${outputsig})") # Output signature

            # Define blocks and connect them
            self.connect()
<% return %>
% else:
            name="${blockname}",
            in_sig="${inputsig}",
            out_sig="${outputsig}${deciminterp}")
% endif

% if blocktype == 'general':
    def forecast(self, noutput_items, ninput_items_required):
        #setup size of input_items[i] for work call
        for i in range(len(ninput_items_required)):
            ninput_items_required[i] = noutput_items

    def general_work(self, input_items, output_items):
        output_items[0][:] = input_items[0]
        consume(0, len(input_items[0]))\
        #self.consume_each(len(input_items[0]))
        return len(output_items[0])
<% return %>
% endif

    def work(self, input_items, output_items):
% if blocktype != 'source':
        in0 = input_items[0]
% endif
% if blocktype != 'sink':
        out = output_items[0]
% endif
        # <+signal processing here+>
% if blocktype in ('sync', 'decimator', 'interpolator'):
        out[:] = in0
        return len(output_items[0])
% elif blocktype == 'sink':
        return len(input_items[0])
% elif blocktype == 'source':
        out[:] = whatever
        return len(output_items[0])
% endif

'''

# C++ file for QA (Boost UTF style)
Templates['qa_cpp_boostutf'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <${include_dir_prefix}/${blockname}.h>
#include <gnuradio/attributes.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_${blockname}_t1)
{
    // Put test here
}

'''

# C++ file for QA
Templates['qa_cpp'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}

#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_${blockname}.h"
#include <${include_dir_prefix}/${blockname}.h>

namespace gr {
  namespace ${modname} {

    void
    qa_${blockname}::t1()
    {
      // Put test here
    }

  } /* namespace ${modname} */
} /* namespace gr */

'''

# Header file for QA
Templates['qa_h'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}

#ifndef _QA_${blockname.upper()}_H_
#define _QA_${blockname.upper()}_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

namespace gr {
  namespace ${modname} {

    class qa_${blockname} : public CppUnit::TestCase
    {
    public:
      CPPUNIT_TEST_SUITE(qa_${blockname});
      CPPUNIT_TEST(t1);
      CPPUNIT_TEST_SUITE_END();

    private:
      void t1();
    };

  } /* namespace ${modname} */
} /* namespace gr */

#endif /* _QA_${blockname.upper()}_H_ */

'''

# Python QA code
Templates['qa_python'] = '''#!/usr/bin/env python
# -*- coding: utf-8 -*-
${str_to_python_comment(license)}
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
% if lang == 'cpp':
from . import ${modname}_swig as ${modname}
% else:
from .${blockname} import ${blockname}
% endif

class qa_${blockname}(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up fg
        self.tb.run()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_${blockname}, "qa_${blockname}.yml")
'''

Templates['grc_yml'] = '''id: ${modname}_${blockname}
label: ${blockname}
category: [${modname}]

templates:
  imports: import ${modname}
  make: ${modname}.${blockname}(${strip_arg_types_grc(arglist)})

<!-- Make one 'parameters' list entry for every Parameter you want settable from the GUI.
     Sub-entries of dictionary:
     * id (makes the value accessible as \$keyname, e.g. in the make entry)
     * label
     * dtype -->
parameters:
- id: ...
  label: ...
  dtype: ...
- id: ...
  label: ...
  dtype: ...

<!-- Make one 'inputs' list entry per input. Sub-entries of dictionary:
     * label (an identifier for the GUI)
     * domain
     * dtype
     * vlen
     * optional (set to 1 for optional inputs) -->
inputs:
- label: ...
  domain: ...
  dtype: ...
  vlen: ...

<!-- Make one 'outputs' list entry per output. Sub-entries of dictionary:
     * label (an identifier for the GUI)
     * dtype
     * vlen
     * optional (set to 1 for optional inputs) -->
- label: ...
  domain: ...
  dtype: !-- e.g. int, float, complex, byte, short, xxx_vector, ...--

file_format: 1
'''

# SWIG string
Templates['swig_block_magic'] = """% if version == '36':
% if blocktype != 'noblock':
GR_SWIG_BLOCK_MAGIC(${modname}, ${blockname});
% endif
%%include "${modname}_${blockname}.h"
% else:
%%include "${include_dir_prefix}/${blockname}.h"
    % if blocktype != 'noblock':
GR_SWIG_BLOCK_MAGIC2(${modname}, ${blockname});
    % endif
% endif
"""

## Old stuff
# C++ file of a GR block
Templates['block_cpp36'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

% if blocktype != 'noblock':
#include <gr_io_signature.h>
% endif
#include "${modname}_${blockname}.h"

% if blocktype == 'noblock':
${modname}_${blockname}::${modname}_${blockname}(${strip_default_values(arglist)})
{
}

${modname}_${blockname}::~${modname}_${blockname}()
{
}
% else:
${modname}_${blockname}_sptr
${modname}_make_${blockname} (${strip_default_values(arglist)})
{
  return gnuradio::get_initial_sptr (new ${modname}_${blockname}(${strip_arg_types(arglist)}));
}

<%
    if blocktype == 'interpolator':
        deciminterp = ', <+interpolation+>'
    elif blocktype == 'decimator':
        deciminterp = ', <+decimation+>'
    else:
        deciminterp = ''
    if arglist == '':
        arglistsep = ''
    else:
        arglistsep = ', '
    if blocktype == 'source':
        inputsig = '0, 0, 0'
    else:
        inputsig = '<+MIN_IN+>, <+MAX_IN+>, sizeof(<+ITYPE+>)'
    endif
    if blocktype == 'sink':
        outputsig = '0, 0, 0'
    else:
        outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, sizeof(<+OTYPE+>)'
    endif
%>

/*
 * The private constructor
 */
${modname}_${blockname}::${modname}_${blockname} (${strip_default_values(arglist)})
  : gr_${grblocktype} ("${blockname}",
       gr_make_io_signature(${inputsig}),
       gr_make_io_signature(${outputsig})${deciminterp})
{
% if blocktype == 'hier'
  connect(self(), 0, d_firstblock, 0);
  // <+connect other blocks+>
  connect(d_lastblock, 0, self(), 0);
% else:
  // Put in <+constructor stuff+> here
% endif
}


/*
 * Our virtual destructor.
 */
${modname}_${blockname}::~${modname}_${blockname}()
{
  // Put in <+destructor stuff+> here
}
% endif


% if blocktype == 'general'
void
${modname}_${blockname}::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int
${modname}_${blockname}::general_work (int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items)
{
  const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
  <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

  // Do <+signal processing+>
  // Tell runtime system how many input items we consumed on
  // each input stream.
  consume_each (noutput_items);

  // Tell runtime system how many output items we produced.
  return noutput_items;
}
% elif blocktype == 'hier' or $blocktype == 'noblock':
% else:
int
${modname}_${blockname}::work(int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items)
{
  const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
  <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

  // Do <+signal processing+>

  // Tell runtime system how many output items we produced.
  return noutput_items;
}
% endif

'''

# Block definition header file (for include/)
Templates['block_h36'] = r'''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}

#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_H
#define INCLUDED_${modname.upper()}_${blockname.upper()}_H

#include <${modname}_api.h>
% if blocktype == 'noblock':
class ${modname.upper()}_API ${blockname}
{
  ${blockname}(${arglist});
  ~${blockname}();
 private:
};

% else:
#include <gr_${grblocktype}.h>

class ${modname}_${blockname};

typedef boost::shared_ptr<${modname}_${blockname}> ${modname}_${blockname}_sptr;

${modname.upper()}_API ${modname}_${blockname}_sptr ${modname}_make_${blockname} (${arglist});

/*!
 * \brief <+description+>
 * \ingroup ${modname}
 *
 */
class ${modname.upper()}_API ${modname}_${blockname} : public gr_${grblocktype}
{
 private:
  friend ${modname.upper()}_API ${modname}_${blockname}_sptr ${modname}_make_${blockname} (${strip_default_values(arglist)});

  ${modname}_${blockname}(${strip_default_values(arglist)});

 public:
  ~${modname}_${blockname}();

  % if blocktype == 'general':
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  // Where all the action really happens
  int general_work (int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
  % elif blocktype == 'hier':
  % else:
  // Where all the action really happens
  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
  % endif
};
% endif

#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_H */

'''
