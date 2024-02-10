# GNU Radio Installer OOT Module Procedure

## Installing OOT Modules
There are two primary methods of installing OOT Modules into an instance of GNU Radio

### GRC Based UI Compainion

This interface queries the https://www.cgran.org/ interface and pulls the OOT Module installer from there

### GNU Radio Installer

Re-run the GNU Radio installer and add the OOT module specified there

### Drive the OOT Module installer yourself (less recommended)

Self explanatory, be sure to point it at the GNU Radio install you want associated with this OOT Module

## Generating Compatible OOT Module installers

There will be a Wix template and some Cpack configuration settings available to incorporated into OOT Module CMake

TODO: instructions on how to incorporate the templates, what to change, and how to generate, point to CPack docs and CMake
tutorial
