juce_add_plugin(HelloDaisySP
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "COCOTONE"                          # Specify the name of the plugin's author
    IS_SYNTH TRUE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Coco               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Jdsp                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "JUCE and DaisySP Example")        # The name of the final executable, which can differ from the target name

juce_generate_juce_header(HelloDaisySP)

target_sources(HelloDaisySP
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Source/PluginEditor.h
        ${CMAKE_CURRENT_LIST_DIR}/Source/PluginEditor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Source/PluginProcessor.h
        ${CMAKE_CURRENT_LIST_DIR}/Source/PluginProcessor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Source/WaveShapeVisualizer.h)

target_compile_definitions(HelloDaisySP
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(HelloDaisySP
    PRIVATE
        DaisySP
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
