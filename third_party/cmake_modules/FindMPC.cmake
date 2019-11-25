# Try to find the MPC library
#
# This module supports requiring a minimum version, e.g. you can do
#   find_package(MPC 1.0.0)
# to require version 1.0.0 to newer of MPC.
#
# Once done, this will define
#
#  MPC_FOUND - system has MPC lib with correct version
#  MPC_INCLUDES - the MPC include directory
#  MPC_LIBRARIES - the MPC library
#  MPC_VERSION - MPC version

# Original Credits for the FindMPFR.cmake script:
# Copyright (c) 2006, 2007 Montel Laurent, <montel@kde.org>
# Copyright (c) 2008, 2009 Gael Guennebaud, <g.gael@free.fr>
# Copyright (c) 2010 Jitse Niesen, <jitse@maths.leeds.ac.uk>
# Copyright (c) 2015 Jack Poulson, <jack.poulson@gmail.com>
# Redistribution and use is allowed according to the terms of the BSD license.

# Modified in 2019 by Electrux for use with MPC, <electruxredsworth@gmail.com>

find_path(MPC_INCLUDES NAMES mpc.h PATHS $ENV{GMPDIR} $ENV{MPCDIR}
  ${INCLUDE_INSTALL_DIR})

# Set MPC_FIND_VERSION to 1.0.0 if no minimum version is specified
if(NOT MPC_FIND_VERSION)
  if(NOT MPC_FIND_VERSION_MAJOR)
    set(MPC_FIND_VERSION_MAJOR 1)
  endif()
  if(NOT MPC_FIND_VERSION_MINOR)
    set(MPC_FIND_VERSION_MINOR 0)
  endif()
  if(NOT MPC_FIND_VERSION_PATCH)
    set(MPC_FIND_VERSION_PATCH 0)
  endif()
  set(MPC_FIND_VERSION
    "${MPC_FIND_VERSION_MAJOR}.${MPC_FIND_VERSION_MINOR}.${MPC_FIND_VERSION_PATCH}")
endif()

if(MPC_INCLUDES)
  # Query MPC_VERSION
  file(READ "${MPC_INCLUDES}/mpc.h" _mpc_version_header)

  string(REGEX MATCH "define[ \t]+MPC_VERSION_MAJOR[ \t]+([0-9]+)"
    _mpc_major_version_match "${_mpc_version_header}")
  set(MPC_MAJOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+MPC_VERSION_MINOR[ \t]+([0-9]+)"
    _mpc_minor_version_match "${_mpc_version_header}")
  set(MPC_MINOR_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "define[ \t]+MPC_VERSION_PATCHLEVEL[ \t]+([0-9]+)"
    _mpc_patchlevel_version_match "${_mpc_version_header}")
  set(MPC_PATCHLEVEL_VERSION "${CMAKE_MATCH_1}")

  set(MPC_VERSION
    ${MPC_MAJOR_VERSION}.${MPC_MINOR_VERSION}.${MPC_PATCHLEVEL_VERSION})

  # Check whether found version exceeds minimum required
  if(${MPC_VERSION} VERSION_LESS ${MPC_FIND_VERSION})
    set(MPC_VERSION_OK FALSE)
    message(STATUS "MPC version ${MPC_VERSION} found in ${MPC_INCLUDES}, "
                   "but at least version ${MPC_FIND_VERSION} is required")
  else()
    set(MPC_VERSION_OK TRUE)
  endif()
endif()

find_library(MPC_LIBRARIES mpc
  PATHS $ENV{MPCDIR} $ENV{MPFRDIR} $ENV{GMPDIR} ${LIB_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPC DEFAULT_MSG
                                  MPC_INCLUDES MPC_LIBRARIES MPC_VERSION_OK)
mark_as_advanced(MPC_INCLUDES MPC_LIBRARIES)