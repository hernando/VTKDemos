#
# VTKDemos
# Copyright (C) 2013 Juan Hernando jhernando@fi.upm.es
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2.1 as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

project(VTKDemos)

###############################################################################
## BUILDYARD BOOTSTRAP

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(Buildyard)
if(BUILDYARD_STOP)
  return()
endif()

###############################################################################
## COMMON OPTIONS

include(Common)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

include_directories(${CMAKE_SOURCE_DIR})
set(_paths_cpp_in ${CMAKE_SOURCE_DIR}/common/paths.cpp.in)
macro(CONFIGURE_PATHS PATHS_FILE)
  set(CONFIG_SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
  set(CONFIG_DATA_PATH ${CMAKE_SOURCE_DIR}/datasets)
  configure_file(${_paths_cpp_in} ${CMAKE_CURRENT_BINARY_DIR}/paths.cpp)

  set(${PATHS_FILE} ${CMAKE_CURRENT_BINARY_DIR}/paths.cpp)
endmacro()

###############################################################################
## EXTERNAL DEPENDENCIES

include(FindPackages)
include(${VTK_USE_FILE})

# This needs to go after FindPackages, so we can't put it in common.
execute_process(COMMAND
  ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1,0,prefix=''))"
  OUTPUT_VARIABLE PYTHON_LIBRARY_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
# Shouldn't the prefix end with site-packages instead of dist-packages
set(PYTHON_LIBRARY_PREFIX "lib/python2.7/site-packages")

message(STATUS "Python Lib Prefix: ${PYTHON_LIBRARY_PREFIX}")

###############################################################################
## SUBPROJECTS

add_subdirectory(intro)
add_subdirectory(ray_cast_spheres)
add_subdirectory(isosurfaces)
add_subdirectory(volume_rendering)
add_subdirectory(streamlines)


