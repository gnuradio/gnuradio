"""Workflow for generating RFNoC bitfiles from GRC.

Copyright 2024 Ettus Research, an NI Brand

This file is part of GNU Radio

SPDX-License-Identifier: GPL-3.0-or-later
"""

import logging
import os
import subprocess
import tempfile
from pathlib import Path

from gnuradio.grc.core import Messages
from gnuradio.grc.core.generator.FlowGraphProxy import FlowGraphProxy
from gnuradio.grc.workflows import GeneratorBase
from gnuradio.grc.workflows.common import add_xterm_to_run_command


def _get_output_path(fg, output_dir):
    """Generate the output path for the image core artefacts.

    - If the user has defined a directory, check it's writable and use it.
    - Otherwise, use the output directory.
    - If the output directory is not writable, use the system temp directory.
    """
    image_core_name = fg.get_option("id")
    if fg.get_option("build_dir"):
        if not os.access(fg.get_option("build_dir"), os.W_OK):
            raise ValueError(
                f"Build directory {fg.get_option('build_dir')} is not writable")
        return os.normpath(os.path.abspath(fg.get_option('build_dir')))
    build_dir_name = "build-" + image_core_name
    if not os.access(output_dir, os.W_OK):
        return os.path.join(tempfile.gettempdir(), build_dir_name)
    return os.path.join(output_dir, build_dir_name)


def generate_build_command(input_file, build_dir, flow_graph):
    """Generate the build command for the RFNoC image."""
    args = ['rfnoc_image_builder', ]
    if flow_graph.get_option('fpga_dir'):
        args.extend(['--fpga-dir', flow_graph.get_option('fpga_dir')])
    build_dir = os.path.normpath(os.path.abspath(build_dir))
    build_output_dir = flow_graph.get_option('build_output') \
        if flow_graph.get_option('build_output') \
        else str(Path(input_file).parent)
    build_ip_dir = flow_graph.get_option('build_ip') \
        if flow_graph.get_option('build_ip') \
        else os.path.join(build_dir, 'build-ip')
    include_dirs = [d.strip() for d in flow_graph.get_option('include_dirs').split(os.pathsep) if d.strip()]
    for include_dir in include_dirs:
        args.extend(['-I', include_dir])
    args.extend([
        "--build-dir", build_dir,
        "--build-output-dir", build_output_dir,
        "--build-ip-dir", build_ip_dir])
    if flow_graph.get_option('jobs'):
        args.extend(['--jobs', str(flow_graph.get_option('jobs'))])
    else:
        args.extend(['--jobs', str(os.cpu_count)])
    if not flow_graph.get_option('include_hash'):
        args.append('--no-hash')
    if not flow_graph.get_option('include_date'):
        args.append('--no-date')
    if flow_graph.get_option('vivado_path'):
        args.extend(['--vivado-path', flow_graph.get_option('vivado_path')])
    if flow_graph.get_option('ignore_warnings'):
        args.append('--ignore-warnings')
    if flow_graph.get_option('reuse'):
        args.append('--reuse')
    args.extend(['--grc-config', input_file])
    return args


class RfnocImageGenerator(GeneratorBase):
    """Generator class for RFNoC bitfiles from GRC."""

    def __init__(self, flow_graph, output_dir):
        """Initialize the RfnocImageGenerator object.

        Args:
            flow_graph: The flow graph to generate the RFNoC image for.
            output_dir: The directory to output the generated image to.
        """
        self.flow_graph = FlowGraphProxy(flow_graph)
        # file_path is the build artifact dir. We call it file_path because GRC
        # expects this attribute and uses it to generate a message.
        self.file_path = _get_output_path(flow_graph, output_dir)
        self.input_file = flow_graph.grc_file_path
        self.log = logging.getLogger(self.__class__.__name__)
        self.xterm = self.flow_graph.parent_platform.config.xterm_executable

    def write(self, called_from_exec=False):
        """Generate the RFNoC image."""
        if called_from_exec:
            self.log.debug("Skipping implied image core generation")
            return
        image_builder_cmd = generate_build_command(
            self.input_file, self.file_path, self.flow_graph) + ['--generate-only']
        self.log.debug("Launching image builder: %s", image_builder_cmd)
        proc_info = subprocess.run(
            image_builder_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
        )
        # FIXME error handling
        if proc_info.stdout:
            Messages.send_verbose_exec(proc_info.stdout)

    def get_exec_args(self):
        """Return a process object that executes the RFNoC image generation."""
        image_builder_cmd = generate_build_command(
            self.input_file, self.file_path, self.flow_graph)
        image_builder_cmd = add_xterm_to_run_command(image_builder_cmd, self.xterm)
        return dict(
            args=image_builder_cmd,
            shell=False,
        )
