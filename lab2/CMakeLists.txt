cmake_minimum_required(VERSION 3.20)
project("Hello, Lab2" LANGUAGES C CXX)

# C/C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Make option() honor normal variables
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# LLVM 19
find_package(LLVM 19 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Keep original global settings
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS})

# Optional but handy for shared/MODULE libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Map commonly used LLVM components to link libs (adjust if needed)
llvm_map_components_to_libnames(LLVM_LIBS
  Core
  Support
  IRReader
  Analysis
  Passes
)

# StaticAnalysisPass
add_llvm_library(StaticAnalysisPass MODULE
  src/StaticAnalysisPass.cpp
  src/Utils.cpp
)
target_compile_features(StaticAnalysisPass PRIVATE cxx_std_17)
target_include_directories(StaticAnalysisPass PRIVATE ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(StaticAnalysisPass PRIVATE ${LLVM_LIBS})

# DynamicAnalysisPass
add_llvm_library(DynamicAnalysisPass MODULE
  src/DynamicAnalysisPass.cpp
  src/Utils.cpp
)
target_compile_features(DynamicAnalysisPass PRIVATE cxx_std_17)
target_include_directories(DynamicAnalysisPass PRIVATE ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(DynamicAnalysisPass PRIVATE ${LLVM_LIBS})

# C runtime shim
add_library(runtime MODULE
  lib/runtime.c
)
target_compile_features(runtime PRIVATE c_std_11)
target_include_directories(runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)