cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(StarflightTLC VERSION 3.0.0)

cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0068 NEW)
cmake_policy(SET CMP0077 NEW)

add_subdirectory(libnoise)
add_subdirectory(lua)
add_subdirectory(src)
