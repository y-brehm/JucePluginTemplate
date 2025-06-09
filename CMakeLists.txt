cmake_minimum_required(VERSION 3.22)
project(PluginTemplate LANGUAGES CXX C)

if(NOT TARGET juce::juce_core)
    add_subdirectory(ThirdParty/JUCE juce_build)
endif()

add_subdirectory(pluginTemplate)
