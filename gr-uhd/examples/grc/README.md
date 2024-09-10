# gr-uhd GRC Examples

This repository contains a collection of GNU Radio Companion (GRC) examples
that demonstrate how to use the gr-uhd module. The gr-uhd module is a GNU Radio
module that provides a set of blocks for interfacing with UHD-compatible
software-defined radios (SDRs).

## General UHD Examples

All examples that start with `uhd_*.grc` are examples that show off the basic
(`multi_usrp`-based) UHD blocks (i.e., the USRP Sink and Source blocks).

The `uhd_fft.grc` and `uhd_siggen_gui.grc` examples are simplified versions of
the command-line utilities with the same name.


## RFNoC Examples

All examples that start with `rfnoc_*.grc` are examples that show off the RFNoC
API. RFNoC (RF Network-on-Chip) is a framework accessing the FPGA within most
USRPs, and allows more fine-grained control over the USRP's signal processing
capabilities.

Most examples require that you are running an FPGA bitfile with the relevant
RFNoC blocks instantiated. For example, the `rfnoc_siggen.grc` will not work
if the SigGen RFNoC block is not present in the device that you are targeting.
On top of that, the blocks must be wired up in a way that is compatible with the
flowgraph you are trying to run.

Many of the `rfnoc_*.grc` show the functionality of one specific RFNoC block in
an isolated manner. For example, `rfnoc_fir_filter.grc` will generate noise in
GNU Radio, send it to the FIR Filter RFNoC block, and then receive the filtered
signal and display it in a frequency sink.

The `rfnoc_duc_radio.grc` and `rfnoc_radio_ddc.grc` examples will work with
standard bitfiles, and use the Radio and DDC/DUC blocks.

A more sophisticated example is `rfnoc_radio_loopback.grc`, which also works
with a standard bitfile on most USRPs. This example demonstrates how to use the
RFNoC API to create a loopback from the TX to the RX chain, which requires
declaring connections as back-edges.

## RFNoC Image Builder Files

The `rfnoc_images` subdirectory contains image builder files. These files are
not meant to be executed within GRC, but are input files for the RFNoC Image
builder. This is an alternative workflow to editing image core YAML files by
hand.

