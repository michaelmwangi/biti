cmake_minimum_required (VERSION 2.6)
project (Biti)

find_library(PthreadLib pthread)
    
FILE(GLOB SOURCES
    *.h
    *.cpp)

add_executable(biti ${SOURCES})
target_link_libraries(biti ${PthreadLib})