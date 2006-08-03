# utilities

from usrp_prims import *

def setup (which_board = 0):
    if not usrp_load_standard_bits (which_board, False):
        raise RuntimeError, "usrp_load_standard_bits"
    dev = usrp_find_device (which_board)
    if not dev:
        raise RuntimeError, "usrp_find_device"
    u = usrp_open_cmd_interface (dev)
    if not u:
        raise RuntimeError, "usrp_open_cmd_interface"

    # FIXME setup high speed paths, Aux ADC Clock, ...

    # usrp_9862_write (u, 0, 35, 0x1)     # aux ADC clock = CLK/4
    # usrp_9862_write (u, 1, 35, 0x1)

    return u

def write_slot_oe (u, slot, value, mask):
    assert 0 <= slot and slot < 4
    return usrp_write_fpga_reg (u, slot + FR_OE_0,
                                ((mask & 0xffff) << 16) | (value & 0xffff))

def write_slot_io (u, slot, value, mask):
    assert 0 <= slot and slot < 4
    return usrp_write_fpga_reg (u, slot + FR_IO_0,
                                ((mask & 0xffff) << 16) | (value & 0xffff))


# ----------------------------------------------------------------


def ramp_aux_dac (u, which_codec, which_dac):
    if not (which_codec == 0 or which_codec == 1):
        raise AssertionError
    if not (which_dac >= 0 and which_dac < 4):
        raise AssertionError
    try:
        if which_dac == 3:                  # sigma delta output
            sigma_delta_loop (u, which_codec)
        else:
            aux_dac_loop (u, which_codec, which_dac)
    except KeyboardInterrupt:
        return

def sigma_delta_loop (u, which_codec):
    counter = 0
    while True:
        usrp_9862_write (u, which_codec, 43, counter >> 4)
        usrp_9862_write (u, which_codec, 42, (counter & 0xf) << 4)
        # counter += 1 FIXME
        counter += 4
        if counter > 0xfff:
            counter = 0

def aux_dac_loop (u, which_codec, which_dac):
    reg = 36 + which_dac                # Aux DAC A,B,C
    counter = 0
    while True:
        usrp_9862_write (u, which_codec, reg, counter)
        counter += 1
        if counter > 0xff:
            counter = 0


def read_aux_adc_loop (u, slot, which_adc):
    while True:
        v = usrp_read_aux_adc (u, slot, which_adc)
        print "%3d  %5.3f" % (v, v * 3.3 / 1024)

def ramp_io_port (u, slot):
    counter = 0
    try:
        while True:
            write_slot_io (u, slot, counter, 0xffff)
            counter += 1
            if counter > 0xffff:
                counter = 0
    except KeyboardInterrupt:
        return

def walk_io_port (u, slot):
    bit = 1
    try:
        while True:
            write_slot_io (u, slot, bit, 0xffff)
            bit = (bit << 1) & 0xffff
            if bit == 0:
                bit = 1
    except KeyboardInterrupt:
        return

