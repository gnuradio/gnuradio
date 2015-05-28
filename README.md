ESA Summer of Code in Space 2015 POLAR Code implementation for GNU Radio
================

This is a GNU Radio clone which contains all the files for my POLAR code project for ESA Summer of Code in Space 2015. My name is Johannes Demel and I am a Master degree student at *Karlsruhe Institute of Technology (KIT)*.

Basics
----------
The project is based on Erdal Arikan's 2009 paper *Channel Polarization: A Method for Constructing Capacity-Achieving Codes for Symmetric Binary-Input Memoryless Channels*. An in-depth explanation of my project is given in my project proposal <https://github.com/jdemel/socis-proposal>.

Structure
----------
The aim of the project is to add POLAR code encoder, decoder, channel construction capabilities to GNU Radio. POLAR codes are used for channel coding, thus all files shall reside within the *gr-fec* submodule. Also encoder and decoder shall use the FECAPI. The decision to do all development in-tree is driven by previous experience that out-of-tree modules are often terribly hard to merge later on and it may take years to do so.

Project
-------
A quick overview of my project timeline goes as follows.

1. create Python testcode
2. implement encoder and decoder in C++ for FECAPI.
3. optimize blocks and create VOLK kernels.
4. implement channel construction algorithms.

This may get continuously updated and I will add more details during project development.