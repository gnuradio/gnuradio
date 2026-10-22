import os
import shutil
import unittest
import yaml
from gnuradio import gr
import variable_save_restore

class qa_variable_save_restore(unittest.TestCase):

    def setUp(self):
        class MockTopBlock:
            def __init__(self):
                self.__class__.__name__ = "TestTopBlock"
                self.samp_rate = 32000
                self.freq = 101.1
            
            def get_samp_rate(self): return self.samp_rate
            def set_samp_rate(self, val): self.samp_rate = val
            
            def get_freq(self): return self.freq
            def set_freq(self, val): self.freq = val

        self.tb = MockTopBlock()

    def test_001_save_and_restore(self):
        srcdir = os.getenv("srcdir", "/tmp")
        statedir = os.environ.get("GR_STATE_PATH")
        yaml_path = os.path.join(srcdir, "saverestore")
        yaml_file = os.path.join(yaml_path, f"TestTopBlock.yml")
        yaml_file = os.path.expanduser(yaml_file)
        os.makedirs(yaml_path, exist_ok=True)
        
        os.environ["GR_STATE_PATH"] = srcdir
        
        self.vsr = variable_save_restore.variable_save_restore(
            top_block=self.tb,
            slot="test_slot",
            variables="samp_rate,freq"
        )
        
        if os.path.exists(yaml_file):
            os.remove(yaml_file)            
        
        self.vsr.filename = yaml_file
        self.vsr.save()
        
        
        self.assertTrue(os.path.exists(yaml_file), f"YAML file not found at {yaml_file}")
        with open(yaml_file, "r") as f:
            data = yaml.safe_load(f)

        self.assertEqual(data["test_slot"]["samp_rate"], 32000)
        self.tb.samp_rate = 99999
        self.vsr.restore()
        
        self.assertEqual(self.tb.samp_rate, 32000)
        
        if os.path.exists(yaml_path):
            shutil.rmtree(yaml_path)
            
        if statedir:
            os.environ["GR_STATE_PATH"] = statedir
    

if __name__ == '__main__':
    unittest.main()