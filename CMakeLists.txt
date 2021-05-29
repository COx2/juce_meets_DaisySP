cmake_minimum_required(VERSION 3.15)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14) # C++14...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

project(juce_meets_DaisySP VERSION 0.0.1)

add_subdirectory(External/JUCE)
add_subdirectory(External/DaisySP)

include(Examples/HelloDaisySP/HelloDaisySP.cmake)
include(Examples/DrumKit/DrumKit.cmake)
