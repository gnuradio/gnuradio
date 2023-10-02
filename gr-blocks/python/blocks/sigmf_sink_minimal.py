#!/usr/bin/env python
#
# Copyright 2022 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from . import blocks_python as blocks
import numpy as np
import json


class sigmf_sink_minimal(gr.hier_block2):
    """
    A partial implementation of the SigMF standard to allow saving SigMF files from GNU Radio.
    It is expected that this block will be replaced with gr-sigmf in the near future, and thus
    it is immediately being depreciated.
    For more information on SigMF see https://github.com/gnuradio/SigMF

    Args:
        item_size: complex, float, or short is all that is supported at the moment
        filename: filename, NOT including the extension, SigMF uses .sigmf-meta and .sigmf-data which will be automatically added to your filename
        sample_rate: sample rate in Hz
        center_freq: optional center Frequency in Hz
        author: optional string used to record author of recording
        description: optional string used to store whatever other info you want about the recording
        hw_info: optional string used to record hardware used in making of recording, e.g. SDR type and antenna, or whether it's simulated data
    """

    def __init__(self, item_size, filename, sample_rate, center_freq, author, description, hw_info, is_complex):

        # Input type, which is included in .sigmf-meta file
        if item_size == 8:
            datatype_str = 'cf32_le'
        elif item_size == 4:
            datatype_str = 'rf32_le'
        elif item_size == 2 and is_complex:
            datatype_str = 'ci16_le'
        elif item_size == 2 and not is_complex:
            datatype_str = 'ri16_le'
        else:
            raise ValueError

        if '.sigmf' in filename:
            filename = filename.rsplit('.', 1)[0]
        gr.log.info(f'Generating {filename}.sigmf-meta, writing SigMF data to {filename}.sigmf-data')

        gr.hier_block2.__init__(self, "sigmf_sink_minimal",
                                gr.io_signature(1, 1, item_size),
                                gr.io_signature(0, 0, 0))

        # use regular File Sink, no append
        file_sink = blocks.file_sink(
            item_size, filename + '.sigmf-data', False)
        self.connect(self, file_sink)

        # JSON/Meta Stuff
        sigmf_version = "1.0.0"  # update me if the time comes

        # Create .sigmf-meta file
        meta_dict = {
            "global": {
                "core:datatype": datatype_str,
                "core:sample_rate": float(sample_rate),
                "core:version": sigmf_version
            },
            "captures": [
                {
                    "core:sample_start": 0,
                }
            ],
            "annotations": []
        }
        if center_freq is not np.nan:
            meta_dict["captures"][0]["core:frequency"] = float(center_freq)
        if hw_info:
            meta_dict["global"]["core:hw"] = str(hw_info)
        if author:
            meta_dict["global"]["core:author"] = str(author)
        if description:
            meta_dict["global"]["core:description"] = str(description)

        with open(filename + '.sigmf-meta', 'w') as f_meta:
            json.dump(meta_dict, f_meta, indent=2)
            f_meta.write('\n')
