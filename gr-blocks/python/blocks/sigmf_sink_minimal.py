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
import numpy as np
import json

class sigmf_sink_minimal(gr.sync_block):
    """
    A partial implementation of the SigMF standard to allow saving SigMF files from GNU Radio.
    It is expected that this block will be replaced with gr-sigmf in the near future, and thus
    it is immediately being depreciated.
    
    Args:
        item_size: Complex or Float stream is all that is currently supported
        filename: Filename, NOT including the extension, SigMF uses .sigmf-meta and .sigmf-data which will be automatically added to your filename
        sample_rate: Sample Rate in Hz
        center_freq: Center Frequency in Hz
        author: optional string used to record author of recording
        description: optional string used to store whatever other info you want about the recording
        hw_info: optional string used to record hardware used in making of recording, e.g. SDR type and antenna, or whether it's simulated data
    """

    def __init__(self, item_size, filename, sample_rate, center_freq, author, description, hw_info):
    
        # Input type, which is included in .sigmf-meta file
        if item_size == 8:
            dtype = np.complex64
            datatype_str = 'cf32_le'
        elif item_size == 4:
            dtype = np.float32
            datatype_str = 'rf32_le'
        else:
            raise ValueError

        gr.sync_block.__init__(self,
                               name = "sigmf_sink_minimal",
                               in_sig = [dtype], # input sig
                               out_sig = None) # no outputs

        sigmf_version = "1.0.0" # update me if the time comes
        
        # Create .sigmf-meta file
        meta_dict = {
            "global": {
                "core:datatype": datatype_str,
                "core:sample_rate": float(sample_rate),
                "core:hw": str(hw_info),
                "core:author": str(author),
                "core:description": str(description),
                "core:version": sigmf_version
            },
            "captures": [
                {
                    "core:sample_start": 0,
                    "core:frequency": float(center_freq)
                }
            ],
            "annotations": []
        }
        with open(filename + '.sigmf-meta', 'w') as f_meta:
            json.dump(meta_dict, f_meta, indent=2)

        # Open .sigmf-data file
        self.f_data = open(filename + '.sigmf-data', "w") # overwrite file

        
    def work(self, input_items, output_items):
        input_items[0].tofile(self.f_data)
        return len(input_items[0])
    
    def stop(self):
        self.f_data.close()

