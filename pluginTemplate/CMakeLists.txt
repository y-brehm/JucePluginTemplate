# These values will be used by juce_add_plugin and embedded into your plugin.
# Remember to customize these for each new plugin you derive from this template!
set(PLUGIN_NAME "PluginTemplate")
set(PLUGIN_VERSION "0.0.1")
set(PLUGIN_MANUFACTURER_NAME "YourName/Company")
set(PLUGIN_MANUFACTURER_CODE "Manu") # 4-character code (e.g., letters from your company name)
set(PLUGIN_CODE "YbPt")              # 4-character unique plugin code
set(PLUGIN_DESCRIPTION "A basic gain plugin with a Web UI.")
set(PLUGIN_URL "yourwebsite.com")
set(PLUGIN_EMAIL "your.email@example.com")
set(PLUGIN_BUNDLE_ID "com.yourcompany.PluginTemplate")

# copy JUCE frontend library to plugin UI files
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/juce/modules/juce_gui_extra/native/javascript/" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/WebUI/js/juce/")

# install Windows webView requirements
if(WIN32)
    find_program(NUGET_EXE NAMES nuget)
    if(NOT NUGET_EXE)
        message(FATAL_ERROR "NUGET.EXE not found. Please install this executable and add it to your PATH, then run CMake again.")
    endif()
    execute_process(COMMAND ${NUGET_EXE} install "Microsoft.Web.WebView2" -OutputDirectory ${CMAKE_BINARY_DIR}/packages
                    RESULT_VARIABLE NUGET_RESULT
                    ERROR_VARIABLE NUGET_ERROR)
    if(NOT NUGET_RESULT EQUAL 0)
         message(FATAL_ERROR "Failed to install WebView2 package using NuGet. Error: ${NUGET_ERROR}")
    endif()
    set(WEBVIEW2_NUGET_VERSION "1.0.2478.35")
    set(WEBVIEW2_NUGET_BASE_PATH "${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2.${WEBVIEW2_NUGET_VERSION}")
    set(JUCE_WEBVIEW2_PACKAGE_LOCATION "${CMAKE_BINARY_DIR}/packages")
endif()

# --- Binary Data for Web UI ---
# This command embeds your HTML/CSS/JS files into the plugin binary.
# The files will be accessible in C++ via the 'BinaryData' namespace.
# For example, 'WebUI/index.html' becomes 'BinaryData::index_html'.
# 1. Define source and destination directories for the UI
set(WEB_UI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/WebUI")
set(WEB_UI_DIST_DIR "${CMAKE_CURRENT_BINARY_DIR}/WebUI_dist")

# 2. Create a custom target to "build" the UI.
#    For a real project, this would be `npm run build`. Here, we just copy the files.
add_custom_target(BuildWebApp ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${WEB_UI_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${WEB_UI_SOURCE_DIR}/" "${WEB_UI_DIST_DIR}/"
    COMMENT "Building Web UI by copying files to dist directory"
)

# 3. Define the output zip file path
set(WEB_UI_ZIP_FILE "${CMAKE_CURRENT_BINARY_DIR}/WebUI.zip")

# 4. Create a custom command to zip the contents of the "dist" directory.
#    This command runs after the BuildWebApp target completes.
add_custom_command(
    OUTPUT ${WEB_UI_ZIP_FILE}
    COMMAND ${CMAKE_COMMAND} -E tar "cf" "${WEB_UI_ZIP_FILE}" --format=zip "."
    WORKING_DIRECTORY ${WEB_UI_DIST_DIR}
    DEPENDS BuildWebApp
    COMMENT "Zipping Web UI assets into ${WEB_UI_ZIP_FILE}"
    VERBATIM
)

# 6. Embed the final zip file as binary data.
juce_add_binary_data(WebUIDataEmbed
    HEADER_NAME WebUIDataEmbed.h
    SOURCES ${WEB_UI_ZIP_FILE}
)

add_subdirectory(pluginTemplateCore)

juce_add_plugin(${PLUGIN_NAME}
    VERSION ${PLUGIN_VERSION}
    COMPANY_NAME "${PLUGIN_MANUFACTURER_NAME}"
    COMPANY_WEBSITE "${PLUGIN_URL}"
    COMPANY_EMAIL "${PLUGIN_EMAIL}"
    PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}
    PLUGIN_CODE ${PLUGIN_CODE}
    # Specify plugin formats (can be a list: VST3 AU AAX Standalone etc.)
    FORMATS VST3 AU

    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    IS_SYNTHESIZER FALSE
    VST3_REPLACE_VST2 OFF
    EDITOR_AVAILABLE TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE

    PLUGIN_AU_BUNDLE_ID ${PLUGIN_BUNDLE_ID}
    PLUGIN_VST3_CATEGORIES "Fx"             
    PLUGIN_DESCRIPTION ${PLUGIN_DESCRIPTION}
    NEEDS_WEB_BROWSER TRUE
    NEEDS_WEBVIEW2 TRUE
)

add_dependencies(${PLUGIN_NAME} WebUIDataEmbed)
# Needs to be deactivated for JUCE 8.0.8
# target_compile_definitions(${PLUGIN_NAME}_VST3 PRIVATE JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1)
target_compile_definitions(${PLUGIN_NAME} 
    PUBLIC 
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
    )

if(TARGET ${PLUGIN_NAME})
    target_sources(${PLUGIN_NAME}
        PRIVATE 
            "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginProcessor.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/source/PluginEditor.cpp"
    )

    target_include_directories(${PLUGIN_NAME} 
        PRIVATE 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_BINARY_DIR}"
    )
else()
    message(WARNING "Target ${PLUGIN_NAME} does not exist, cannot manually add sources.")
endif()

set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PLUGIN_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(${PLUGIN_NAME}
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats      
        juce::juce_audio_processors   
        juce::juce_audio_utils        
        juce::juce_core               
        juce::juce_dsp
        juce::juce_data_structures    
        juce::juce_events             
        juce::juce_graphics           
        juce::juce_gui_basics         
        juce::juce_gui_extra          
        pluginTemplateCore
        WebUIDataEmbed                
)

# --- Platform-Specific Settings ---
if(APPLE)
    set_target_properties(${PLUGIN_NAME}_VST3 PROPERTIES BUNDLE_EXTENSION "vst3")
    set_target_properties(${PLUGIN_NAME}_AU PROPERTIES BUNDLE_EXTENSION "component")
endif()

