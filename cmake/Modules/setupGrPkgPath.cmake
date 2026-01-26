# Setup GNU Radio installation directory paths for GNU Radio scripts
# Makes scripts find their installation directory automatically

# Copyright 2025 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

message(STATUS "Setting up portable paths...")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Create Python code that finds the GNU Radio installation directory
# Gets injected into scripts as @GR_PKG_DIR_SETUP@
set(GR_PKG_DIR_SETUP "import os; os.environ.setdefault('GR_PKG_DIR', os.path.dirname(os.path.dirname(os.path.abspath(__file__))))")

message(STATUS "GNU Radio installation directory ready")