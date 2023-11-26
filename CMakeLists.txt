cmake_minimum_required(VERSION 2.8.12.2)
project(mymuduo)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/library)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

aux_source_directory(net SRC_LIST1)
aux_source_directory(base SRC_LIST2)
set(SRC_LIST ${SRC_LIST1} ${SRC_LIST2})

include_directories(${CMAKE_SOURCE_DIR}/base)
include_directories(${CMAKE_SOURCE_DIR}/net)

add_library(mymuduo SHARED ${SRC_LIST})
