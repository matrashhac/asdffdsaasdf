# Install script for directory: /1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/xilinx/Vitis/2023.2/gnu/microblaze/lin/bin/mb-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/libsrc/common/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/libsrc/gpio/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/libsrc/intc/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/libsrc/tmrctr/src/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/libsrc/uartlite/src/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/include/../include")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/include/.." TYPE DIRECTORY MESSAGE_LAZY FILES "/1work/fpgaproc/driver_for_both_ips/platform/microblaze_0/standalone_microblaze_0/bsp/libsrc/build_configs/gen_bsp/include")
endif()

