import os
import shlex
import subprocess
import threading
import time
from pathlib import Path
from shutil import which as find_executable
from gnuradio import gr

from ...core import Messages
from ...core.utils.system import get_cmake_nproc


class ExecFlowGraphThread(threading.Thread):
    """Execute the flow graph as a new process and wait on it to finish."""

    def __init__(self, view, flowgraph, xterm_executable, callback):
        """
        ExecFlowGraphThread constructor.
        """
        threading.Thread.__init__(self)

        self.view = view
        self.flow_graph = flowgraph
        self.xterm_executable = xterm_executable or gr.prefs().get_string('grc', 'xterm_executable', 'xterm')
        self.update_callback = callback

        try:
            if self.flow_graph.get_option('output_language') == 'python':
                self.process = self.view.process = self._popen()
            elif self.flow_graph.get_option('output_language') == 'cpp':
                self.process = self.view.process = self._cpp_popen()

            self.update_callback()
            self.start()
        except Exception as e:
            Messages.send_verbose_exec(str(e))
            Messages.send_end_exec()

    def _popen(self):
        """
        Execute this python flow graph.
        """
        generator = self.view.get_generator()
        run_command = self.flow_graph.get_run_command(generator.file_path)
        run_command_args = shlex.split(run_command)

        # When in no gui mode on linux, use a graphical terminal (looks nice)
        xterm_executable = find_executable(self.xterm_executable)
        if generator.generate_options == 'no_gui' and xterm_executable:
            if ('gnome-terminal' in xterm_executable):
                run_command_args = [xterm_executable, '--'] + run_command_args
            else:
                run_command_args = [xterm_executable, '-e'] + run_command_args

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
        generator = self.view.get_generator()
        run_command = generator.file_path + \
            '/build/' + self.flow_graph.get_option('id')

        dirname = generator.file_path
        builddir = os.path.join(dirname, 'build')

        if os.path.isfile(run_command):
            os.remove(run_command)

        xterm_executable = find_executable(self.xterm_executable)

        nproc = get_cmake_nproc()

        run_command_args = f'cmake .. && cmake --build . -j{nproc} && {xterm_executable} -e {os.path.abspath(run_command)}'
        Messages.send_start_exec(run_command_args)

        return subprocess.Popen(
            args=run_command_args,
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
            Messages.send_verbose_exec(r)
            r = self.process.stdout.read(1)

        # Properly close pipe before thread is terminated
        self.process.stdout.close()
        while self.process.poll() is None:
            # Wait for the process to fully terminate
            time.sleep(0.05)

        self.done

    def done(self):
        """Perform end of execution tasks."""
        Messages.send_end_exec(self.process.returncode)
        self.view.process = None
        self.update_callback()
