#!/usr/bin/env python
# Copyright 2026 Chie4
# Copyright 2026 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later


import logging
import pathlib
import shutil
import tempfile
from urllib import parse, request
from functools import cache
from os import getenv
from typing import Optional

from gnuradio import gr_unittest
from gnuradio import gr, blocks, dtv

URLS = {
    "ofdm symbol acquisition": {
        "url": "https://gr-pubbuck.s3.us-west-002.backblazeb2.com/dtv-ofdm-sym-acquisition.fc32",
        "comment": "kindly provided by Chie4 in https://github.com/gnuradio/gnuradio/pull/8124",
    }
}


@cache
def online_tests_enabled() -> bool:
    enabled = getenv("GR_ONLINE_TESTS")
    return enabled and not (
        enabled.upper() in ("NO", "DISABLED", "OFF", "0", "OFFLINE")
    )


@cache
def download_dir() -> pathlib.Path:
    target_dir = getenv("GR_DTV_DOWNLOADS_DIR")
    if not target_dir:
        target_dir = tempfile.mkdtemp(prefix="gr_online_test_", suffix=".tmp")
    return pathlib.Path(target_dir)


@cache
def fetch_online(name: str) -> Optional[pathlib.Path]:
    """Fetch all online resources and return whether that succeeded.

    If it succeeded, cache the result. We don't want to re-download for every
    test method, so we can't do this in TestCase.__init__ or .setUp"""
    url = URLS[name]["url"]
    target_dir = download_dir()
    parts = parse.urlsplit(url)
    filename = pathlib.PosixPath(parts.path).parts[-1]
    local_file = target_dir / filename
    if local_file.exists():
        logging.info(f"returning pre-downloaded {local_file}")
        return local_file
    if not online_tests_enabled():
        logging.warning(
            "Online tests disabled " +
            f'(environment variable "GR_ONLINE_TESTS") and {filename} not pre-downloaded'
        )
        return None
    try:
        logging.info(f"opening {url}")
        with request.urlopen(url) as response:
            with open(local_file, "wb") as f:
                logging.info(f"writing {url} to {local_file}")
                shutil.copyfileobj(response, f)
        logging.info(f"closed {local_file}")
        return local_file
    except Exception as e:
        logging.warning(f"Couldn't download {url}: {e}")


class online_tests(gr_unittest.TestCase):
    downloaded_files = {"dummy": None}

    def setUp(self):
        self.tb = gr.top_block()

    @gr_unittest.skipIf(
        not fetch_online("ofdm symbol acquisition"),
        "couldn't download",
    )
    def test_001_OFDM_symbol_acquisition(self):
        """Test against a known recording.

        This led to segfaults as reported in #8124.
        """
        fname = fetch_online("ofdm symbol acquisition")
        src = blocks.file_source(gr.sizeof_gr_complex, str(fname), False)
        acq = dtv.dvbt_ofdm_sym_acquisition(1, 2048, 1705, 64, 8.0)
        null = blocks.null_sink(gr.sizeof_gr_complex * 2048)
        self.tb.connect(src, acq, null)
        self.tb.run()


if __name__ == "__main__":
    gr_unittest.run(online_tests)
