import os
import sys
import tempfile
import subprocess
import textwrap
from pathlib import Path
from gnuradio import gr_unittest

class test_variable_save_restore(gr_unittest.TestCase):

    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.env = os.environ.copy()
        self.current_dir = Path(__file__).parent.absolute()
        
        # Isolate environment to clear compiler cache
        self.env.update({
            "GR_STATE_PATH": self.temp_dir.name,
            "GR_PREFS_PATH": self.temp_dir.name,
            "XDG_CACHE_HOME": self.temp_dir.name
        })
        
        # Inject local uninstalled block definitions into the GRC search path
        grc_blocks_dir = self.current_dir.parent.parent / "grc"
        existing_paths = self.env.get("GRC_BLOCKS_PATH", "")
        self.env["GRC_BLOCKS_PATH"] = f"{grc_blocks_dir}:{existing_paths}"

        self.grc_path = Path(self.temp_dir.name) / "qa_variable_save_restore.grc"
        self._write_mock_flowgraph()

    def tearDown(self):
        self.temp_dir.cleanup()

    def _write_mock_flowgraph(self):
        """Generates the minimal headless flowgraph layout."""
        layout = textwrap.dedent("""\
            metadata:
              file_format: 1
            options:
              parameters:
                id: qa_save_restore
                generate_options: no_gui
                output_language: python
                run: 'True'
              states:
                coordinate: [8, 8]
                state: enabled

            blocks:
            - name: target_variable
              id: variable
              parameters:
                value: '1337'
              states:
                coordinate: [200, 12]
                state: enabled

            - name: variable_save_restore_0
              id: variable_save_restore
              parameters:
                description_variable: '""'
                restore_trigger: 'False'
                save_trigger: 'False'
                show_msg_ports: 'No'
                slot: '"qa_slot"'
                variables: '"target_variable"'
              states:
                coordinate: [368, 12]
                state: enabled

            connections: []
        """)
        with open(self.grc_path, "w") as f:
            f.write(layout)

    def test_001_grcc_compilation_and_execution(self):
        # Phase 1: Compile the .grc to .py
        try:
            subprocess.run(
                ["grcc", str(self.grc_path)],
                cwd=self.temp_dir.name, 
                env=self.env,
                capture_output=True,
                text=True,
                check=True
            )
        except subprocess.CalledProcessError as e:
            self.fail(f"Compiler exception.\nSTDOUT:\n{e.stdout}\nSTDERR:\n{e.stderr}")

        # Phase 2: Monkey-patch environment and execute simulated UI triggers
        driver_code = textwrap.dedent(f"""\
            import sys
            
            # Force local module resolution
            sys.path.insert(0, '{self.current_dir}')
            from gnuradio import blocks
            import variable_save_restore
            
            # Hook the execution path to use local uninstalled payload
            blocks.variable_save_restore = variable_save_restore.variable_save_restore
            blocks.saveRestoreVariables = variable_save_restore.variable_save_restore
            
            # Execute generated flowgraph
            sys.path.insert(0, '{self.temp_dir.name}')
            import qa_save_restore
            
            tb = qa_save_restore.qa_save_restore()
            
            if hasattr(tb, 'variable_save_restore_0'):
                block = tb.variable_save_restore_0
                if hasattr(block, 'set_save_trigger'):
                    block.set_save_trigger(True)
                    tb.set_target_variable(9999)
                    block.set_restore_trigger(True)
                    print(f"RESTORED_VALUE:{{tb.get_target_variable()}}")
                    sys.exit(0)
            
            print("RESTORED_VALUE:FAILED")
        """)
        
        driver_path = Path(self.temp_dir.name) / "driver.py"
        with open(driver_path, "w") as f:
            f.write(driver_code)

        # Phase 3: Assert Execution
        try:
            result = subprocess.run(
                [sys.executable, str(driver_path)],
                env=self.env,
                capture_output=True,
                text=True,
                check=True
            )
            self.assertIn("RESTORED_VALUE:1337", result.stdout, f"State restore failed.\nLog:\n{result.stdout}")
        except subprocess.CalledProcessError as e:
            self.fail(f"Driver exception.\nSTDOUT:\n{e.stdout}\nSTDERR:\n{e.stderr}")

if __name__ == '__main__':
    gr_unittest.run(test_variable_save_restore)
