cmake_minimum_required(VERSION 3.10)

find_package(LLVM 8 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

option(USE_REFERENCE "Build with reference solution" OFF)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})


add_executable(fuzzer
  src/Fuzzer.cpp
  src/Utils.cpp
  )

add_llvm_library(InstrumentPass MODULE
  src/Instrument.cpp
  )

add_library(runtime MODULE
  lib/runtime.c
  )
