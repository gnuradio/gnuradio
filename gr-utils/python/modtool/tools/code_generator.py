#
# Copyright 2013-2014 Free Software Foundation, Inc.
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
""" A code generator (needed by ModToolAdd) """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

from mako.template import Template
from ..templates import Templates
from .util_functions import str_to_fancyc_comment
from .util_functions import str_to_python_comment
from .util_functions import strip_default_values
from .util_functions import strip_arg_types
from .util_functions import strip_arg_types_grc

GRTYPELIST = {
    'sync': 'sync_block',
    'sink': 'sync_block',
    'source': 'sync_block',
    'decimator': 'sync_decimator',
    'interpolator': 'sync_interpolator',
    'general': 'block',
    'tagged_stream': 'tagged_stream_block',
    'hier': 'hier_block2',
    'noblock': ''
}

def render_template(tpl_id, **kwargs):
    """ Return the parsed and rendered template given by tpl_id """
    # Choose template
    tpl = Template(Templates[tpl_id])
    # Set up all variables
    kwargs['str_to_fancyc_comment'] = str_to_fancyc_comment
    kwargs['str_to_python_comment'] = str_to_python_comment
    kwargs['strip_default_values'] = strip_default_values
    kwargs['strip_arg_types'] = strip_arg_types
    kwargs['strip_arg_types_grc'] = strip_arg_types_grc
    kwargs['grblocktype'] = GRTYPELIST[kwargs['blocktype']]
    if kwargs['is_component']:
        kwargs['include_dir_prefix'] = "gnuradio/" + kwargs['modname']
    else:
        kwargs['include_dir_prefix'] = kwargs['modname']
    # Render and return
    return tpl.render(**kwargs)

