# Copyright 2008-2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


def hide_bokeh_gui_options_if_not_installed(options_blk):
    try:
        import bokehgui
    except ImportError:
        for param in options_blk.parameters_data:
            if param['id'] == 'generate_options':
                ind = param['options'].index('bokeh_gui')
                del param['options'][ind]
                del param['option_labels'][ind]
