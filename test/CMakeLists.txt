cmake_minimum_required (VERSION 2.6)
project (Bititest)

    
FILE(GLOB SOURCES
    *.h
    *.cpp)

add_executable(bititest ${SOURCES})
