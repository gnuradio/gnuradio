# Copyright 2024 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#[=======================================================================[.rst:
FindGtk
-------------

Find the GTK C++ and Python includes, library, and modules.

Note: Currently the module is intended to support Windows only
as installing and detecting the GTK is difficult.

This module will find and load the GTK into CMake, but its primary
purpose is determining the location of the GTK bin directory
so we can be sure it's added to the PATH before our GI python checks
and so we can install/distribute the GTK alongside our distributions

This module will search under common Windows install prefixes
    AND
all paths under the GTK_ROOT cmake/environment variable (cmake preffered)
With order:
    $GTK_ROOT\bin (cmake)
    $GTK_ROOT\bin (env)
    C:\gtk-build\gtk\(?:x64|x86)\release\bin
    C:\Program Files\gtk\bin
    C:\Program Files (x86)\gtk\bin
    C:\gtk\bin

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``Gtk::Gtk``
  The Gtk library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``gtk_FOUND``
  True if the system has GTK+3.
``gtk_VERSION``
  The version of GTK+3 that was found.
``gtk_INCLUDE_DIRS``
  Include directories needed to use GTK+3.
``gtk_LIBRARIES``
  Libraries needed to link to GTK+3.

#]=======================================================================]


# Find the GTK installation root
find_path(GTK_BIN_DIR
NAMES
    gobject-2.0-0.dll
PATHS
    ${GTK_ROOT}/bin
    $ENV{GTK_ROOT}/bin
    "C:/gtk-build/gtk/x64/release/bin"
    "C:/Program Files/gtk/bin"
    "C:/Program Files (x86)/gtk/bin"
    "C:/gtk/bin"
)

# Find the libraries
find_library(gdk_lib
NAMES
    gdk-3.dll
    gdk-3-vs17.dll
HINTS
    ${GTK_BIN_DIR}
)

find_library(girepository_lib
NAMES
    girepository
    girepository-1.0-1
HINTS
    ${GTK_BIN_DIR}
)


