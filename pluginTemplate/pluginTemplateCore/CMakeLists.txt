project(pluginTemplateCore)

add_library(pluginTemplateCore STATIC)

target_sources(pluginTemplateCore 
    PRIVATE
    source/DspProcessor.cpp
    source/PeakLevelMeter.cpp
    )
target_include_directories(pluginTemplateCore
    PUBLIC
        include
)

target_link_libraries(pluginTemplateCore
    PUBLIC
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
    )

target_compile_features(pluginTemplateCore PUBLIC cxx_std_17)
