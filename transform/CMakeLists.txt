project(change-type CXX)
cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM "/opt/homebrew/Cellar/llvm/16.0.1")

set(CMAKE_MODULE_PATH "${LLVM}/lib/cmake" "${LLVM}/lib/cmake/llvm" "${LLVM}/lib/cmake/clang")

include(llvm/AddLLVM)
include(clang/AddClang)

set(LLVM_LINK_COMPONENTS Support)
add_library(change-type SHARED change-type.cc)
target_compile_options(change-type PRIVATE "-std=c++17")
target_include_directories(change-type PRIVATE ${LLVM}/include)
target_link_directories(change-type PRIVATE ${LLVM}/lib)
target_link_libraries(change-type PRIVATE LLVM-16 clang-cpp ncurses)
target_link_options(change-type PRIVATE -undefined dynamic_lookup)

add_executable(autogen-builder autogen-builder.cc)
target_compile_options(autogen-builder PRIVATE "-std=c++17")
target_include_directories(autogen-builder PRIVATE ${LLVM}/include)
target_link_directories(autogen-builder PRIVATE ${LLVM}/lib)
target_link_libraries(autogen-builder PRIVATE LLVM-16 clang-cpp ncurses)
target_link_options(autogen-builder PRIVATE -undefined dynamic_lookup)

add_executable(test test.cc test2.cc)
target_compile_options(test PRIVATE "-std=c++17")
