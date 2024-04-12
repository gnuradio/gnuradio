#
# Copyright 2013-2014, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
''' All the templates for skeleton files (needed by ModToolAdd) '''


from datetime import datetime

Templates = {}

# Default licence
Templates['defaultlicense'] = '''
Copyright %d {copyrightholder}.

SPDX-License-Identifier: GPL-3.0-or-later
''' % datetime.now().year

Templates['grlicense'] = f'''
Copyright {datetime.now().year} Free Software Foundation, Inc.

This file is part of GNU Radio

SPDX-License-Identifier: GPL-3.0-or-later
'''

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
  % if blocktype != "source":
    #pragma message("set the following appropriately and remove this warning")
    using input_type = float;
  % endif
  % if blocktype != "sink":
    #pragma message("set the following appropriately and remove this warning")
    using output_type = float;
  % endif
    ${blockname}::sptr
    ${blockname}::make(${strip_default_values(arglist)})
    {
      return gnuradio::make_block_sptr<${blockname}_impl>(
        ${strip_arg_types(arglist)});
    }

<%
    if blocktype == 'decimator':
    #pragma message("set the following appropriately and remove this warning")
        decimation = ', 2 /*<+decimation+>*/'
    elif blocktype == 'interpolator':
    #pragma message("set the following appropriately and remove this warning")
        decimation = ', 2 /*<+interpolation+>*/'
    elif blocktype == 'tagged_stream':
    #pragma message("set the following appropriately and remove this warning")
        decimation = ', /*<+len_tag_key+>*/'
    else:
        decimation = ''
    endif
    if blocktype == 'source':
        inputsig = '0, 0, 0'
    else:
        inputsig = '1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)'
    endif
    if blocktype == 'sink':
        outputsig = '0, 0, 0'
    else:
        outputsig = '1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)'
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
    #pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    ${blockname}_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
    % if blocktype != 'source':
      auto in = static_cast<const input_type*>(input_items[0]);
    % endif
    % if blocktype != 'sink':
      auto out = static_cast<output_type*>(output_items[0]);
    % endif

      #pragma message("Implement the signal processing in your block and remove this warning")
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
      #pragma message("set the following appropriately and remove this warning")
      int noutput_items = 0;
      return noutput_items ;
    }

    int
    ${blockname}_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
    % if blocktype != 'source':
      auto in = static_cast<const input_type*>(input_items[0]);
    % endif
    % if blocktype != 'sink':
      auto out = static_cast<output_type*>(output_items[0]);
    % endif

      #pragma message("Implement the signal processing in your block and remove this warning")
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
      auto in = static_cast<const input_type*>(input_items[0]);
    % endif
    % if blocktype != 'sink':
      auto out = static_cast<output_type*>(output_items[0]);
    % endif

      #pragma message("Implement the signal processing in your block and remove this warning")
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
      typedef std::shared_ptr<${blockname}> sptr;

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
        inputsig = '[<+numpy.float32+>, ]'
    if blocktype == 'sink':
        outputsig = 'None'
    else:
        outputsig = '[<+numpy.float32+>, ]'
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
        deciminterp = ', interp=<+interpolation+>'
    elif blocktype == 'decimator':
        deciminterp = ', decim=<+decimation+>'
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
            gr.io_signature(${inputsig}),  # Input signature
            gr.io_signature(${outputsig})) # Output signature

        # Define blocks and connect them
        self.connect()
<% return %>
% else:
            name="${blockname}",
            in_sig=${inputsig},
            out_sig=${outputsig}${deciminterp})
% endif

% if blocktype == 'general':
    def forecast(self, noutput_items, ninputs):
        # ninputs is the number of input connections
        # setup size of input_items[i] for work call
        # the required number of input items is returned
        #   in a list where each element represents the
        #   number of required items for each input
        ninput_items_required = [noutput_items] * ninputs
        return ninput_items_required

    def general_work(self, input_items, output_items):
        # For this sample code, the general block is made to behave like a sync block
        ninput_items = min([len(items) for items in input_items])
        noutput_items = min(len(output_items[0]), ninput_items)
        output_items[0][:noutput_items] = input_items[0][:noutput_items]
        self.consume_each(noutput_items)
        return noutput_items

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
#include <${include_dir_prefix}/${blockname}.h>
#include <gnuradio/attributes.h>
#include <boost/test/unit_test.hpp>

namespace gr {
  namespace ${modname} {

    BOOST_AUTO_TEST_CASE(test_${blockname}_replace_with_specific_test_name)
    {
      // Put test here
    }

  } /* namespace ${modname} */
} /* namespace gr */
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
from gnuradio import gr, gr_unittest
# from gnuradio import blocks
% if lang == 'cpp':
try:
% if version in ['310']:
    from gnuradio.${modname} import ${blockname}
% else:
    from ${modname} import ${blockname}
% endif
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
% if version in ['310']:
    from gnuradio.${modname} import ${blockname}
% else:
    from ${modname} import ${blockname}
% endif
% else:
% if version in ['310']:
from gnuradio.${modname} import ${blockname}
% else:
from ${blockname} import ${blockname}
% endif
% endif

class qa_${blockname}(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        # FIXME: Test will fail until you pass sensible arguments to the constructor
        instance = ${blockname}()

    def test_001_descriptive_test_name(self):
        # set up fg
        self.tb.run()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_${blockname})
'''

Templates['grc_yml'] = '''id: ${modname}_${blockname}
label: ${blockname}
category: '[${modname}]'

templates:
% if version in ['310']:
  imports: from gnuradio import ${modname}
% else:
  imports: import ${modname}
% endif
  make: ${modname}.${blockname}(${strip_arg_types_grc(arglist)})

#  Make one 'parameters' list entry for every parameter you want settable from the GUI.
#     Keys include:
#     * id (makes the value accessible as keyname, e.g. in the make entry)
#     * label (label shown in the GUI)
#     * dtype (e.g. int, float, complex, byte, short, xxx_vector, ...)
#     * default
parameters:
- id: parametername_replace_me
  label: FIX ME
  dtype: string
  default: You need to fill in your grc/${modname}_${blockname}.block.yaml
#- id: ...
#  label: ...
#  dtype: ...

#  Make one 'inputs' list entry per input and one 'outputs' list entry per output.
#  Keys include:
#      * label (an identifier for the GUI)
#      * domain (optional - stream or message. Default is stream)
#      * dtype (e.g. int, float, complex, byte, short, xxx_vector, ...)
#      * vlen (optional - data stream vector length. Default is 1)
#      * optional (optional - set to 1 for optional inputs. Default is 0)
inputs:
#- label: ...
#  domain: ...
#  dtype: ...
#  vlen: ...
#  optional: ...

outputs:
#- label: ...
#  domain: ...
#  dtype: ...
#  vlen: ...
#  optional: ...

#  'file_format' specifies the version of the GRC yml format used in the file
#  and should usually not be changed.
file_format: 1
'''

# Old stuff
# C++ file of a GR block
Templates['block_cpp36'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment(license)}
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

typedef std::shared_ptr<${modname}_${blockname}> ${modname}_${blockname}_sptr;

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
