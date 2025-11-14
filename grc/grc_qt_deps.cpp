/*
 * Copyright 2025 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*
 * GRC Qt Dependencies Helper Library
 * This minimal library ensures that Qt5PrintSupport.dll gets bundled
 * with GNU Radio installations for use by PyQt5/QtPy in GRC.
 * The GNU Radio bundle system automatically discovers and copies DLLs
 * that are runtime dependencies of built libraries. Since GRC (Python)
 * uses QtPy which needs Qt5PrintSupport for PDF generation, but this
 * isn't linked by any C++ GNU Radio libraries, this helper library
 * creates the necessary dependency chain.
 */

#include <QtPrintSupport/QPrinter>

extern "C" {

#ifdef _WIN32
__declspec(dllexport)
#endif
    void grc_ensure_qt_print_support()
{
    // Create a QPrinter instance to ensure Qt5PrintSupport.dll is linked
    // This function doesn't need to be called - just its existence triggers linking
    QPrinter printer;
    (void)printer; // Suppress unused variable warning
}

} // extern "C"