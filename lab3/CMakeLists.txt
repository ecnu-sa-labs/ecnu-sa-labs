cmake_minimum_required(VERSION 3.20)
project("Hello, Lab3" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(LLVM 19 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

llvm_map_components_to_libnames(LLVM_LIBS
  Core
  Support
  IRReader
  Analysis
  Passes
)

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

