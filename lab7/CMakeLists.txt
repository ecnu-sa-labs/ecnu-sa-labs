cmake_minimum_required(VERSION 3.10)

find_package(LLVM 8 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

set(Z3_DIR $ENV{Z3_DIR})
get_filename_component(Z3_DIR ${Z3_DIR} ABSOLUTE)
set(Z3_CXX_INCLUDE_DIRS ${Z3_DIR}/include)
set(Z3_LIBRARIES ${Z3_DIR}/bin/libz3.so)
message(STATUS "Z3_DIR: ${Z3_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
include_directories(${Z3_CXX_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${Z3_LIBRARIES})

add_executable(dse
  src/DSE.cpp
  src/Strategy.cpp
  )

add_llvm_library(InstrumentPass MODULE
  src/DSEInstrument.cpp
  )

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(dse ${llvm_libs} ${Z3_LIBRARIES})

add_library(runtime MODULE
  src/SymbolicInterpreter.cpp
  src/Runtime.cpp
  )

target_link_libraries(runtime ${llvm_libs} ${Z3_LIBRARIES})
