# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import ast
import os
import sys
import unittest.mock
import yaml

if 'gnuradio' not in sys.modules:
    sys.modules['gnuradio'] = unittest.mock.MagicMock()

from grc.core.platform import Platform


def test_dynamic_channel_model_codegen():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '../..'))
    yml_path = os.path.join(repo_root, 'gr-channels', 'grc', 'channels_dynamic_channel_model.block.yml')
    assert os.path.exists(yml_path), f"File {yml_path} does not exist"

    p = Platform(version='3.11', install_prefix='/usr')
    with open(yml_path, 'r') as f:
        data = yaml.safe_load(f)
    p.load_block_description(data, yml_path)
    fg = p.make_flow_graph()
    blk = p.make_block(fg, 'channels_dynamic_channel_model')
    make_code = blk.templates.render('make').strip()

    parsed = ast.parse(make_code)
    call = parsed.body[0].value
    # Constructor of channels.dynamic_channel_model expects 14 arguments:
    # samp_rate, sro_std_dev, sro_max_dev, cfo_std_dev, cfo_max_dev, N,
    # doppler_freq, LOS_model, K, delays, mags, ntaps_mpath, noise_amp, noise_seed
    assert len(call.args) == 14
