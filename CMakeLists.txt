# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# project name and languages
project(cli CXX)

##############################################################################

# cli.exe
add_executable(cli cli.cpp)

# starter.exe
add_executable(starter starter.cpp)

# canceler.exe
add_executable(canceler canceler.cpp)

##############################################################################
