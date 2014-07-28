# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

if(DEFINED __INCLUDED_GR_USEQT4_CMAKE)
    return()
endif()
set(__INCLUDED_GR_USEQT4_CMAKE TRUE)

# This file is derived from the default "UseQt4" file provided by
# CMake.  This version sets the variables "QT_INCLUDE_DIRS",
# "QT_LIBRARIES", and "QT_LIBRARIES_PLUGINS" depending on those
# requested during the "find_package(Qt4 ...)" function call, but
# without actually adding them to the include or library search
# directories ("include_directories" or "link_directories").  The
# adding in is done by the CMakeLists.txt build scripts in the using
# project.

# Copyright from the original file, as required by the license.
################################################################
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2011 Kitware, Inc., Insight Software Consortium
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# # A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
################################################################

ADD_DEFINITIONS(${QT_DEFINITIONS})
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG QT_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO QT_NO_DEBUG)
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_MINSIZEREL QT_NO_DEBUG)
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS QT_NO_DEBUG)
ENDIF()

SET(QT_INCLUDE_DIRS ${QT_INCLUDE_DIR})
SET(QT_LIBRARIES "")
SET(QT_LIBRARIES_PLUGINS "")

IF (QT_USE_QTMAIN)
  IF (Q_WS_WIN)
    SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
  ENDIF (Q_WS_WIN)
ENDIF (QT_USE_QTMAIN)

IF(QT_DONT_USE_QTGUI)
  SET(QT_USE_QTGUI 0)
ELSE(QT_DONT_USE_QTGUI)
  SET(QT_USE_QTGUI 1)
ENDIF(QT_DONT_USE_QTGUI)

IF(QT_DONT_USE_QTCORE)
  SET(QT_USE_QTCORE 0)
ELSE(QT_DONT_USE_QTCORE)
  SET(QT_USE_QTCORE 1)
ENDIF(QT_DONT_USE_QTCORE)

IF (QT_USE_QT3SUPPORT)
  ADD_DEFINITIONS(-DQT3_SUPPORT)
ENDIF (QT_USE_QT3SUPPORT)

# list dependent modules, so dependent libraries are added
SET(QT_QT3SUPPORT_MODULE_DEPENDS QTGUI QTSQL QTXML QTNETWORK QTCORE)
SET(QT_QTSVG_MODULE_DEPENDS QTGUI QTXML QTCORE)
SET(QT_QTUITOOLS_MODULE_DEPENDS QTGUI QTXML QTCORE)
SET(QT_QTHELP_MODULE_DEPENDS QTGUI QTSQL QTXML QTNETWORK QTCORE)
IF(QT_QTDBUS_FOUND)
  SET(QT_PHONON_MODULE_DEPENDS QTGUI QTDBUS QTCORE)
ELSE(QT_QTDBUS_FOUND)
  SET(QT_PHONON_MODULE_DEPENDS QTGUI QTCORE)
ENDIF(QT_QTDBUS_FOUND)
SET(QT_QTDBUS_MODULE_DEPENDS QTXML QTCORE)
SET(QT_QTXMLPATTERNS_MODULE_DEPENDS QTNETWORK QTCORE)
SET(QT_QAXCONTAINER_MODULE_DEPENDS QTGUI QTCORE)
SET(QT_QAXSERVER_MODULE_DEPENDS QTGUI QTCORE)
SET(QT_QTSCRIPTTOOLS_MODULE_DEPENDS QTGUI QTCORE)
SET(QT_QTWEBKIT_MODULE_DEPENDS QTXMLPATTERNS QTGUI QTCORE)
SET(QT_QTDECLARATIVE_MODULE_DEPENDS QTSCRIPT QTSVG QTSQL QTXMLPATTERNS QTGUI QTCORE)
SET(QT_QTMULTIMEDIA_MODULE_DEPENDS QTGUI QTCORE)
SET(QT_QTOPENGL_MODULE_DEPENDS QTGUI QTCORE)
SET(QT_QTSCRIPT_MODULE_DEPENDS QTCORE)
SET(QT_QTGUI_MODULE_DEPENDS QTCORE)
SET(QT_QTTEST_MODULE_DEPENDS QTCORE)
SET(QT_QTXML_MODULE_DEPENDS QTCORE)
SET(QT_QTSQL_MODULE_DEPENDS QTCORE)
SET(QT_QTNETWORK_MODULE_DEPENDS QTCORE)

# Qt modules  (in order of dependence)
FOREACH(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
               QAXSERVER QAXCONTAINER QTDECLARATIVE QTSCRIPT QTSVG QTUITOOLS QTHELP
               QTWEBKIT PHONON QTSCRIPTTOOLS QTMULTIMEDIA QTXMLPATTERNS QTGUI QTTEST
               QTDBUS QTXML QTSQL QTNETWORK QTCORE)

  IF (QT_USE_${module} OR QT_USE_${module}_DEPENDS)
    IF (QT_${module}_FOUND)
      IF(QT_USE_${module})
        STRING(REPLACE "QT" "" qt_module_def "${module}")
        ADD_DEFINITIONS(-DQT_${qt_module_def}_LIB)
	SET(QT_INCLUDE_DIRS ${QT_INCLUDE_DIRS} ${QT_${module}_INCLUDE_DIR})
      ENDIF(QT_USE_${module})
      SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_${module}_LIBRARY})
      SET(QT_LIBRARIES_PLUGINS ${QT_LIBRARIES_PLUGINS} ${QT_${module}_PLUGINS})
      IF(QT_IS_STATIC)
        SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_${module}_LIB_DEPENDENCIES})
      ENDIF(QT_IS_STATIC)
      FOREACH(depend_module ${QT_${module}_MODULE_DEPENDS})
        SET(QT_USE_${depend_module}_DEPENDS 1)
      ENDFOREACH(depend_module ${QT_${module}_MODULE_DEPENDS})
    ELSE (QT_${module}_FOUND)
      MESSAGE("Qt ${module} library not found.")
    ENDIF (QT_${module}_FOUND)
  ENDIF (QT_USE_${module} OR QT_USE_${module}_DEPENDS)

ENDFOREACH(module)
