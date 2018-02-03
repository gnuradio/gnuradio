#
# Copyright 2013 Free Software Foundation, Inc.
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
Copyright %d Free Software Foundation, Inc.

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
''' % datetime.now().year

# Header file of a sync/decimator/interpolator block
Templates['block_impl_h'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}
\#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H
\#define INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H

\#include <${include_dir_prefix}/${blockname}.h>

namespace gr {
  namespace ${modname} {

    class ${blockname}_impl : public ${blockname}
    {
     private:
      // Nothing to declare in this block.

#if $blocktype == 'tagged_stream'
     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

#end if
     public:
      ${blockname}_impl(${strip_default_values($arglist)});
      ~${blockname}_impl();

      // Where all the action really happens
#if $blocktype == 'general'
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
#else if $blocktype == 'tagged_stream'
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
#else if $blocktype == 'hier'
#silent pass
#else
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
#end if
    };

  } // namespace ${modname}
} // namespace gr

\#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_IMPL_H */

'''

# C++ file of a GR block
Templates['block_impl_cpp'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}
\#ifdef HAVE_CONFIG_H
\#include "config.h"
\#endif

\#include <gnuradio/io_signature.h>
#if $blocktype == 'noblock'
\#include <${include_dir_prefix}/${blockname}.h>
#else
\#include "${blockname}_impl.h"
#end if

namespace gr {
  namespace ${modname} {

#if $blocktype == 'noblock'
    $blockname::${blockname}(${strip_default_values($arglist)})
    {
    }

    $blockname::~${blockname}()
    {
    }
#else
    ${blockname}::sptr
    ${blockname}::make(${strip_default_values($arglist)})
    {
      return gnuradio::get_initial_sptr
        (new ${blockname}_impl(${strip_arg_types($arglist)}));
    }

#if $blocktype == 'decimator'
#set $decimation = ', <+decimation+>'
#else if $blocktype == 'interpolator'
#set $decimation = ', <+interpolation+>'
#else if $blocktype == 'tagged_stream'
#set $decimation = ', <+len_tag_key+>'
#else
#set $decimation = ''
#end if
#if $blocktype == 'source'
#set $inputsig = '0, 0, 0'
#else
#set $inputsig = '<+MIN_IN+>, <+MAX_IN+>, sizeof(<+ITYPE+>)'
#end if
#if $blocktype == 'sink'
#set $outputsig = '0, 0, 0'
#else
#set $outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, sizeof(<+OTYPE+>)'
#end if
    /*
     * The private constructor
     */
    ${blockname}_impl::${blockname}_impl(${strip_default_values($arglist)})
      : gr::${grblocktype}("${blockname}",
              gr::io_signature::make($inputsig),
              gr::io_signature::make($outputsig)$decimation)
#if $blocktype == 'hier'
    {
      connect(self(), 0, d_firstblock, 0);
      // connect other blocks
      connect(d_lastblock, 0, self(), 0);
    }
#else
    {}
#end if

    /*
     * Our virtual destructor.
     */
    ${blockname}_impl::~${blockname}_impl()
    {
    }

#if $blocktype == 'general'
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
#else if $blocktype == 'tagged_stream'
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
#else if $blocktype == 'hier'
#silent pass
#else
    int
    ${blockname}_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
#if $blocktype == 'source'
#silent pass
#else
      const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
#end if
#if $blocktype == 'sink'
#silent pass
#else
      <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];
#end if

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
#end if
#end if

  } /* namespace ${modname} */
} /* namespace gr */

'''

# Block definition header file (for include/)
Templates['block_def_h'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}

\#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_H
\#define INCLUDED_${modname.upper()}_${blockname.upper()}_H

\#include <${include_dir_prefix}/api.h>
#if $blocktype != 'noblock'
\#include <gnuradio/${grblocktype}.h>
#end if

namespace gr {
  namespace ${modname} {

#if $blocktype == 'noblock'
    /*!
     * \\brief <+description+>
     *
     */
    class ${modname.upper()}_API $blockname
    {
    public:
      ${blockname}(${arglist});
      ~${blockname}();
    private:
    };
#else
    /*!
     * \\brief <+description of block+>
     * \ingroup ${modname}
     *
     */
    class ${modname.upper()}_API ${blockname} : virtual public gr::$grblocktype
    {
     public:
      typedef boost::shared_ptr<${blockname}> sptr;

      /*!
       * \\brief Return a shared_ptr to a new instance of ${modname}::${blockname}.
       *
       * To avoid accidental use of raw pointers, ${modname}::${blockname}'s
       * constructor is in a private implementation
       * class. ${modname}::${blockname}::make is the public interface for
       * creating new instances.
       */
      static sptr make($arglist);
    };
#end if

  } // namespace ${modname}
} // namespace gr

\#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_H */

'''

# Python block
Templates['block_python'] = '''\#!/usr/bin/env python
# -*- coding: utf-8 -*-
${str_to_python_comment($license)}
#
#if $blocktype == 'noblock'
#stop
#end if

#if $blocktype in ('sync', 'sink', 'source')
#set $parenttype = 'gr.sync_block'
#else
#set $parenttype = {'hier': 'gr.hier_block2', 'interpolator': 'gr.interp_block', 'decimator': 'gr.decim_block', 'general': 'gr.basic_block'}[$blocktype]
#end if
#if $blocktype != 'hier'
import numpy
#if $blocktype == 'source'
#set $inputsig = 'None'
#else
#set $inputsig = '[<+numpy.float32+>]'
#end if
#if $blocktype == 'sink'
#set $outputsig = 'None'
#else
#set $outputsig = '[<+numpy.float32+>]'
#end if
#else
#if $blocktype == 'source'
#set $inputsig = '0, 0, 0'
#else
#set $inputsig = '<+MIN_IN+>, <+MAX_IN+>, gr.sizeof_<+ITYPE+>'
#end if
#if $blocktype == 'sink'
#set $outputsig = '0, 0, 0'
#else
#set $outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, gr.sizeof_<+OTYPE+>'
#end if
#end if
#if $blocktype == 'interpolator'
#set $deciminterp = ', <+interpolation+>'
#else if $blocktype == 'decimator'
#set $deciminterp = ', <+decimation+>'
#else
#set $deciminterp = ''
#end if
from gnuradio import gr

class ${blockname}(${parenttype}):
    """
    docstring for block ${blockname}
    """
    def __init__(self#if $arglist == '' then '' else ', '#$arglist):
        ${parenttype}.__init__(self,
#if $blocktype == 'hier'
            "$blockname",
            gr.io_signature(${inputsig}),  # Input signature
            gr.io_signature(${outputsig})) # Output signature

            # Define blocks and connect them
            self.connect()
#stop
#else
            name="${blockname}",
            in_sig=${inputsig},
            out_sig=${outputsig}${deciminterp})
#end if

#if $blocktype == 'general'
    def forecast(self, noutput_items, ninput_items_required):
        #setup size of input_items[i] for work call
        for i in range(len(ninput_items_required)):
            ninput_items_required[i] = noutput_items

    def general_work(self, input_items, output_items):
        output_items[0][:] = input_items[0]
        consume(0, len(input_items[0]))
        \#self.consume_each(len(input_items[0]))
        return len(output_items[0])
#stop
#end if

    def work(self, input_items, output_items):
#if $blocktype != 'source'
        in0 = input_items[0]
#end if
#if $blocktype != 'sink'
        out = output_items[0]
#end if
        # <+signal processing here+>
#if $blocktype in ('sync', 'decimator', 'interpolator')
        out[:] = in0
        return len(output_items[0])
#else if $blocktype == 'sink'
        return len(input_items[0])
#else if $blocktype == 'source'
        out[:] = whatever
        return len(output_items[0])
#end if

'''

# C++ file for QA
Templates['qa_cpp'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}

\#include <gnuradio/attributes.h>
\#include <cppunit/TestAssert.h>
\#include "qa_${blockname}.h"
\#include <${include_dir_prefix}/${blockname}.h>

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
${str_to_fancyc_comment($license)}

\#ifndef _QA_${blockname.upper()}_H_
\#define _QA_${blockname.upper()}_H_

\#include <cppunit/extensions/HelperMacros.h>
\#include <cppunit/TestCase.h>

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

\#endif /* _QA_${blockname.upper()}_H_ */

'''

# Python QA code
Templates['qa_python'] = '''\#!/usr/bin/env python
# -*- coding: utf-8 -*-
${str_to_python_comment($license)}
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
#if $lang == 'cpp'
import ${modname}_swig as ${modname}
#else
from ${blockname} import ${blockname}
#end if

class qa_$blockname (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_${blockname}, "qa_${blockname}.xml")
'''

Templates['grc_xml'] = '''<?xml version="1.0"?>
<block>
  <name>$blockname</name>
  <key>${modname}_$blockname</key>
  <category>[$modname]</category>
  <import>import $modname</import>
  <make>${modname}.${blockname}(${strip_arg_types_grc($arglist)})</make>
  <!-- Make one 'param' node for every Parameter you want settable from the GUI.
       Sub-nodes:
       * name
       * key (makes the value accessible as \$keyname, e.g. in the make node)
       * type -->
  <param>
    <name>...</name>
    <key>...</key>
    <type>...</type>
  </param>

  <!-- Make one 'sink' node per input. Sub-nodes:
       * name (an identifier for the GUI)
       * type
       * vlen
       * optional (set to 1 for optional inputs) -->
  <sink>
    <name>in</name>
    <type><!-- e.g. int, float, complex, byte, short, xxx_vector, ...--></type>
  </sink>

  <!-- Make one 'source' node per output. Sub-nodes:
       * name (an identifier for the GUI)
       * type
       * vlen
       * optional (set to 1 for optional inputs) -->
  <source>
    <name>out</name>
    <type><!-- e.g. int, float, complex, byte, short, xxx_vector, ...--></type>
  </source>
</block>
'''

# Usage
Templates['usage'] = '''
gr_modtool <command> [options] -- Run <command> with the given options.
gr_modtool help -- Show a list of commands.
gr_modtool help <command> -- Shows the help for a given command. '''

# SWIG string
Templates['swig_block_magic'] = """#if $version == '36'
#if $blocktype != 'noblock'
GR_SWIG_BLOCK_MAGIC($modname, $blockname);
#end if
%include "${modname}_${blockname}.h"
#else
%include "${include_dir_prefix}/${blockname}.h"
#if $blocktype != 'noblock'
GR_SWIG_BLOCK_MAGIC2($modname, $blockname);
#end if
#end if
"""

## Old stuff
# C++ file of a GR block
Templates['block_cpp36'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}
\#ifdef HAVE_CONFIG_H
\#include "config.h"
\#endif

#if $blocktype != 'noblock'
\#include <gr_io_signature.h>
#end if
\#include "${modname}_${blockname}.h"

#if $blocktype == 'noblock'
${modname}_${blockname}::${modname}_${blockname}(${strip_default_values($arglist)})
{
}

${modname}_${blockname}::~${modname}_${blockname}()
{
}
#else
${modname}_${blockname}_sptr
${modname}_make_${blockname} (${strip_default_values($arglist)})
{
  return gnuradio::get_initial_sptr (new ${modname}_${blockname}(${strip_arg_types($arglist)}));
}

#if $blocktype == 'decimator'
#set $decimation = ', <+decimation+>'
#else if $blocktype == 'interpolator'
#set $decimation = ', <+interpolation+>'
#else
#set $decimation = ''
#end if
#if $blocktype == 'sink'
#set $inputsig = '0, 0, 0'
#else
#set $inputsig = '<+MIN_IN+>, <+MAX_IN+>, sizeof(<+ITYPE+>)'
#end if
#if $blocktype == 'source'
#set $outputsig = '0, 0, 0'
#else
#set $outputsig = '<+MIN_OUT+>, <+MAX_OUT+>, sizeof(<+OTYPE+>)'
#end if

/*
 * The private constructor
 */
${modname}_${blockname}::${modname}_${blockname} (${strip_default_values($arglist)})
  : gr_${grblocktype} ("${blockname}",
       gr_make_io_signature($inputsig),
       gr_make_io_signature($outputsig)$decimation)
{
#if $blocktype == 'hier'
  connect(self(), 0, d_firstblock, 0);
  // <+connect other blocks+>
  connect(d_lastblock, 0, self(), 0);
#else
  // Put in <+constructor stuff+> here
#end if
}


/*
 * Our virtual destructor.
 */
${modname}_${blockname}::~${modname}_${blockname}()
{
  // Put in <+destructor stuff+> here
}
#end if


#if $blocktype == 'general'
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
#else if $blocktype == 'hier' or $blocktype == 'noblock'
#pass
#else
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
#end if

'''

# Block definition header file (for include/)
Templates['block_h36'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}

\#ifndef INCLUDED_${modname.upper()}_${blockname.upper()}_H
\#define INCLUDED_${modname.upper()}_${blockname.upper()}_H

\#include <${modname}_api.h>
#if $blocktype == 'noblock'
class ${modname.upper()}_API $blockname
{
  ${blockname}(${arglist});
  ~${blockname}();
 private:
};

#else
\#include <gr_${grblocktype}.h>

class ${modname}_${blockname};

typedef boost::shared_ptr<${modname}_${blockname}> ${modname}_${blockname}_sptr;

${modname.upper()}_API ${modname}_${blockname}_sptr ${modname}_make_${blockname} ($arglist);

/*!
 * \\brief <+description+>
 * \ingroup ${modname}
 *
 */
class ${modname.upper()}_API ${modname}_${blockname} : public gr_$grblocktype
{
 private:
  friend ${modname.upper()}_API ${modname}_${blockname}_sptr ${modname}_make_${blockname} (${strip_default_values($arglist)});

  ${modname}_${blockname}(${strip_default_values($arglist)});

 public:
  ~${modname}_${blockname}();

#if $blocktype == 'general'
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  // Where all the action really happens
  int general_work (int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
#else if $blocktype == 'hier'
#pass
#else
  // Where all the action really happens
  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
#end if
};
#end if

\#endif /* INCLUDED_${modname.upper()}_${blockname.upper()}_H */

'''

# C++ file for QA
Templates['qa_cpp36'] = '''/* -*- c++ -*- */
${str_to_fancyc_comment($license)}

\#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(qa_${modname}_${blockname}_t1){
    BOOST_CHECK_EQUAL(2 + 2, 4);
    // TODO BOOST_* test macros here
}

BOOST_AUTO_TEST_CASE(qa_${modname}_${blockname}_t2){
    BOOST_CHECK_EQUAL(2 + 2, 4);
    // TODO BOOST_* test macros here
}

'''

# Header file for QA
Templates['qa_cmakeentry36'] = """
add_executable($basename $filename)
target_link_libraries($basename gnuradio-$modname \${Boost_LIBRARIES})
GR_ADD_TEST($basename $basename)
"""

