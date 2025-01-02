#
# Copyright 2013-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" A code generator (needed by ModToolAdd) """


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
    include_prefix = kwargs["includedir"].split("/")[1:]  # expecting "include/gnuradio/modname"
    kwargs["include_dir_prefix"] = "/".join(include_prefix)
    kwargs["module_import_py"] = ".".join(include_prefix)
    kwargs["module_import_grc"] = "from " + ".".join(include_prefix[:-1]) if len(include_prefix) > 1 else ""
    kwargs["module_import_grc"] += f"import {include_prefix[-1]}"
    # Render and return
    return tpl.render(**kwargs)
