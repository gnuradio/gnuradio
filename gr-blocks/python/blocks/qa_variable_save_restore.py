#!/usr/bin/env python

from gnuradio import gr_unittest
from gnuradio.blocks.variable_save_restore import variable_save_restore
import os
import yaml
import tempfile
from gnuradio import gr


class TestTopBlock(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)
        self.gain = 10

    def get_gain(self):
        return self.gain

    def set_gain(self, val):
        self.gain = val


class qa_variable_save_restore(gr_unittest.TestCase):

    def test_save_and_restore_variable(self):

        tmp_path = tempfile.mkdtemp()

        os.environ["GR_PREFS_PATH"] = tmp_path

        tb = TestTopBlock()

        vsr = variable_save_restore(
            top_block=tb,
            slot="default",
            variables="gain"
        )

        # save variable
        vsr.save()

        yaml_file = os.path.join(
            str(tmp_path),
            "saverestore",
            "TestTopBlock.yml"
        )

        self.assertTrue(os.path.exists(yaml_file))

        with open(yaml_file, "r") as f:
            data = yaml.safe_load(f)

        # check saved value
        self.assertEqual(data["default"]["gain"], 10)

        # change variable
        tb.set_gain(0)

        # restore variable
        vsr.restore()

        # verify restored value
        self.assertEqual(tb.get_gain(), 10)


if __name__ == '__main__':
    gr_unittest.run(qa_variable_save_restore)
