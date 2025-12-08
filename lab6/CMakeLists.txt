cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(LLVM 19 REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

option(USE_REFERENCE "Build with reference solution" OFF)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/reference)


if (USE_REFERENCE)

  find_library(HiddenAnalysis libHiddenAnalysis.a)

  message(STATUS "Use reference solution")
  add_llvm_library(DivZeroPass MODULE
    src/DivZeroAnalysis.cpp
    src/Transfer.cpp
    src/Domain.cpp
    src/Utils.cpp
  )

  target_link_libraries(DivZeroPass PRIVATE HiddenAnalysis)

else (USE_REFERENCE)
  add_llvm_library(DivZeroPass MODULE
  src/DivZeroAnalysis.cpp
  src/Transfer.cpp
  src/ChaoticIteration.cpp
  src/Domain.cpp
  src/Utils.cpp
  )
endif (USE_REFERENCE)