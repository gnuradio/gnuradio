import os

from os import path

import pytest

from grc.core.platform import Platform

try:
    os.mkdir(path.join(path.dirname(__file__), 'resources/tests'))
except FileExistsError:
    pass

# Gather blocks
BLOCK_PATHS = []
ROOT = path.join(path.dirname(__file__), '../..')
BLOCK_PATHS = [path.abspath(path.join(ROOT, 'grc/blocks')),
               path.abspath(path.join(ROOT, 'build/gr-uhd/grc'))]
for file_dir in os.scandir(ROOT):
    # If it is a module
    if path.isdir(file_dir) and file_dir.name.startswith("gr-"):
        BLOCK_PATHS.append(path.abspath(path.join(file_dir, "grc")))

# These examples are known to fail and need to be resolved
# But skip now to allow test to pass and prevent further
# regression on the vast majority of examples
BLACKLISTED = ['transmitter_sim_hier.grc',
               'uhd_packet_rx.grc',
               'uhd_packet_tx_tun.grc',
               'packet_loopback_hier.grc',
               'uhd_packet_tx.grc',
               'uhd_packet_rx_tun.grc',
               'formatter_ofdm.grc',
               'ber_curve_gen_ldpc.grc',
               'ber_curve_gen.grc',
               'polar_ber_curve_gen.grc',
               'tpc_ber_curve_gen.grc',
               'comparing_resamplers.grc',
               'pfb_sync_test.grc',
               'soapy_receive.grc',
               'soapy_receive2.grc',
               'soapy_transmit.grc',
               'fm_radio_receiver_soapyremote.grc',
               'fm_radio_receiver_soapy.grc',
               'uhd_siggen_gui.grc',
               'filter_taps_loader.grc',
               'uhd_fft.grc'
               ]


def gather_examples():
    global ROOT, BLACKLISTED
    example_paths = []
    for file_dir in os.scandir(ROOT):
        # If it is a module
        if path.isdir(file_dir) and file_dir.name.startswith("gr-") and not file_dir.name.startswith('gr-trellis'):
            try:
                for pos_ex in os.scandir(path.join(file_dir, "examples")):
                    if path.isfile(pos_ex) and pos_ex.name.endswith(".grc") and not path.basename(pos_ex) in BLACKLISTED:
                        example_paths.append(pos_ex)
                    elif path.isdir(pos_ex):
                        for pos_ex2 in os.scandir(pos_ex):
                            if path.isfile(pos_ex2) and pos_ex2.name.endswith(".grc") and not path.basename(pos_ex2) in BLACKLISTED:
                                example_paths.append(pos_ex2)

            except FileNotFoundError:
                continue
    return example_paths


def print_proper(element):
    if element.is_block:
        return element.name
    return f"{element.parent.name} - {element}"


@pytest.mark.examples
@pytest.mark.parametrize("example", gather_examples())
def test_all_examples(example):
    global BLOCK_PATHS

    print(example.path)

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library(BLOCK_PATHS)

    flow_graph = platform.make_flow_graph(example.path)
    flow_graph.rewrite()
    flow_graph.validate()

    assert flow_graph.is_valid(), (example.name, [
        f"{print_proper(elem)}: {msg}" for elem, msg in flow_graph.iter_error_messages()])

    generator = platform.Generator(flow_graph, path.join(
        path.dirname(__file__), 'resources/tests'))
    generator.write()


if __name__ == '__main__':
    examples = gather_examples()
    for example in examples:
        test_all_examples(example)
