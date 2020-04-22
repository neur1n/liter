cmake_minimum_required(VERSION 2.8.2)

project(liter)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(
  ${WINDOWS_SDK_INCLUDE_DIR}/um
  include
)

link_directories(
  ${WINDOWS_SDK_LIB_DIR}/um/x86
)

add_executable(liter src/main.cpp src/liter.cpp)
