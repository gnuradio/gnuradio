# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

import os
import shlex
import subprocess
import threading
import time
from distutils.spawn import find_executable

from gi.repository import GLib

from ..core import Messages


class ExecFlowGraphThread(threading.Thread):
    """Execute the flow graph as a new process and wait on it to finish."""

    def __init__(self, flow_graph_page, xterm_executable, callback):
        """
        ExecFlowGraphThread constructor.
        """
        threading.Thread.__init__(self)

        self.page = flow_graph_page  # store page and don't use main window calls in run
        self.flow_graph = self.page.flow_graph
        self.xterm_executable = xterm_executable
        self.update_callback = callback

        try:
            if self.flow_graph.get_option('output_language') == 'python':
                self.process = self.page.process = self._popen()
            elif self.flow_graph.get_option('output_language') == 'cpp':
                self.process = self.page.process = self._cpp_popen()

            self.update_callback()
            self.start()
        except Exception as e:
            Messages.send_verbose_exec(str(e))
            Messages.send_end_exec()

    def _popen(self):
        """
        Execute this python flow graph.
        """
        generator = self.page.get_generator()
        run_command = self.flow_graph.get_run_command(generator.file_path)
        run_command_args = shlex.split(run_command)

        # When in no gui mode on linux, use a graphical terminal (looks nice)
        xterm_executable = find_executable(self.xterm_executable)
        if generator.generate_options == 'no_gui' and xterm_executable:
            run_command_args = [xterm_executable, '-e', run_command]

        # this does not reproduce a shell executable command string, if a graphical
        # terminal is used. Passing run_command though shlex_quote would do it but
        # it looks really ugly and confusing in the console panel.
        Messages.send_start_exec(' '.join(run_command_args))

        return subprocess.Popen(
            args=run_command_args,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            shell=False, universal_newlines=True
        )

    def _cpp_popen(self):
        """
        Execute this C++ flow graph after generating and compiling it.
        """
        generator = self.page.get_generator()
        run_command = generator.file_path + '/build/' + self.flow_graph.get_option('id')

        dirname = generator.file_path
        builddir = os.path.join(dirname, 'build')

        if os.path.isfile(run_command):
            os.remove(run_command)

        xterm_executable = find_executable(self.xterm_executable)

        run_command_args = ['cmake .. &&', 'make && ', xterm_executable, '-e', run_command]
        Messages.send_start_exec(' '.join(run_command_args))

        return subprocess.Popen(
            args=' '.join(run_command_args),
            cwd=builddir,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            shell=True, universal_newlines=True
        )

    def run(self):
        """
        Wait on the executing process by reading from its stdout.
        Use GObject.idle_add when calling functions that modify gtk objects.
        """
        # handle completion
        r = "\n"
        while r:
            GLib.idle_add(Messages.send_verbose_exec, r)
            r = self.process.stdout.read(1)

        # Properly close pipe before thread is terminated
        self.process.stdout.close()
        while self.process.poll() is None:
            # Wait for the process to fully terminate
            time.sleep(0.05)

        GLib.idle_add(self.done)

    def done(self):
        """Perform end of execution tasks."""
        Messages.send_end_exec(self.process.returncode)
        self.page.process = None
        self.update_callback()
