juce_add_plugin(DrumKit
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
    PLUGIN_CODE Drum                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "DrumKit")        # The name of the final executable, which can differ from the target name

juce_generate_juce_header(DrumKit)

### Find source files with generate source groups ###
# Test that the target_path should be excluded
# if the path is excluded, IS_EXCLUDED will be set to TRUE
function(TEST_EXCLUDED TARGET_PATH EXCLUSION_PATTERN_LIST IS_EXCLUDED)
  set(IS_EXCLUDED FALSE PARENT_SCOPE)
  foreach(PAT ${EXCLUSION_PATTERN_LIST})
    if(TARGET_PATH MATCHES ${PAT})
      set(IS_EXCLUDED TRUE PARENT_SCOPE)
      break()
    endif()
  endforeach()
endfunction()

# set target extension list
set(TARGET_SOURCE_PATTERNS "*.c" "*.cc" "*.cpp" "*.h" "*.hpp")

if(MSVC)
  list(APPEND TARGET_SOURCE_PATTERNS "*.rc")
elseif(XCODE)
  list(APPEND TARGET_SOURCE_PATTERNS "*.mm")
endif()

# generate_source_list(
#   output
#   BASE_DIR dir
#   SOURCE_DIRS dir1 [dir2...]
#   EXCLUSION_PATTERNS [pattern1...]
#   )
function(generate_source_list output)
  cmake_parse_arguments(GSL "" "BASE_DIR" "SOURCE_DIRS;EXCLUSION_PATTERNS" ${ARGN})

  if(NOT DEFINED GSL_BASE_DIR)
    message(FATAL_ERROR "BASE_DIR must be specified.")
  endif()

  if(NOT DEFINED GSL_SOURCE_DIRS)
    message(FATAL_ERROR "SOURCE_DIRS must be specified.")
  endif()

  message("Print EXCLUSION_PATTERNS: ${GSL_EXCLUSION_PATTERNS}")

  # set the root directory of the source file tree in IDE
  get_filename_component(GSL_BASE_DIR "${GSL_BASE_DIR}" ABSOLUTE)

  foreach(SOURCE_DIR ${GSL_SOURCE_DIRS})
    get_filename_component(SOURCE_DIR "${SOURCE_DIR}" ABSOLUTE)

    set(PATTERNS "")
    foreach(PATTERN ${TARGET_SOURCE_PATTERNS})
      list(APPEND PATTERNS "${SOURCE_DIR}/${PATTERN}")
    endforeach()

    file(GLOB_RECURSE FILES ${PATTERNS})

    # Define SourceGroup reflecting filesystem hierarchy.
    foreach(FILE_PATH ${FILES})
      get_filename_component(FILEPATH "${FILE_PATH}" ABSOLUTE)
      TEST_EXCLUDED(${FILE_PATH} "${GSL_EXCLUSION_PATTERNS}" IS_EXCLUDED)
      if(IS_EXCLUDED)
        continue()
      endif()

      get_filename_component(PARENT_DIR "${FILE_PATH}" DIRECTORY)
      file(RELATIVE_PATH GROUP_NAME "${GSL_BASE_DIR}" "${PARENT_DIR}")
      string(REPLACE "/" "\\" GROUP_NAME "${GROUP_NAME}")
      source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
      list(APPEND SOURCE_FILES "${FILE_PATH}")
    endforeach()
  endforeach()

  message("Files ${SOURCE_FILES}")
  set(${output} "${SOURCE_FILES}" PARENT_SCOPE)
endfunction()

generate_source_list(
    DrumKit_SOURCES
    BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/Source"
    SOURCE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Source"
#     EXCLUSION_PATTERNS ".+/Test/.+"
    )

target_sources(DrumKit
    PRIVATE
        ${DrumKit_SOURCES}
    )

###^^^ Find source files with generate source groups ^^^###

target_compile_definitions(DrumKit
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(DrumKit
    PRIVATE
        DaisySP
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
