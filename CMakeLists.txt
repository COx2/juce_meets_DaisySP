cmake_minimum_required(VERSION 3.15)

project(juce_meets_DaisySP VERSION 0.0.1)

add_subdirectory(External/JUCE)
add_subdirectory(External/DaisySP)

include(Examples/HelloDaisySP/HelloDaisySP.cmake)
include(Examples/DrumKit/DrumKit.cmake)
