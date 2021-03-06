
# Copyright (c) 2013-2014, EPFL/Blue Brain Project
#                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>
#
# Generates executables for the unit tests and defines a "tests" make target
# which runs all of them.

# TEST_LIBRARIES is used by CommonCTest.cmake to link tests against them
set(TEST_LIBRARIES mock ${DC_LIBRARIES} ${Boost_LIBRARIES})

find_package(X11)
if(NOT X11_FOUND)
  list(APPEND EXCLUDE_FROM_TESTS core/WebbrowserTests.cpp)
endif()
if(NOT BUILD_CORE_LIBRARY)
  list(APPEND EXCLUDE_FROM_TESTS
    common/PixelStreamSegmentDecoderTests.cpp
    core/AsciiToQtKeyCodeMapperTests.cpp
    core/CommandLineOptionsTests.cpp
    core/CommandTests.cpp
    core/ConfigurationTests.cpp
    core/ConfigurationTests.cpp
    core/ContentWindowTests.cpp
    core/DockToolbarTests.cpp
    core/DoubleTapGestureTests.cpp
    core/LocalPixelStreamerTests.cpp
    core/PixelStreamBufferTests.cpp
    core/PixelStreamWindowManagerTests.cpp
    core/StateSerializationTests.cpp
    core/TextInputHandlerTests.cpp
    core/WebbrowserTests.cpp
    core/WebkitHtmlSelectReplacementTests.cpp
    dcWebservice/BoostRegexTests.cpp
    dcWebservice/HandlerTests.cpp
    dcWebservice/MapperTests.cpp
    dcWebservice/RequestBuilderTests.cpp
    dcWebservice/ResponseTests.cpp
    dcWebservice/ServerTests.cpp
)
endif()

if(NOT BUILD_CORE_LIBRARY)
  file(GLOB DC_COMMON_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} common/*.cpp)
  file(GLOB DC_CORE_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} core/*.cpp)
  file(GLOB DC_WEBSERVICE_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} dcWebservice/*.cpp)
  list(APPEND EXCLUDE_FROM_TESTS
    ${DC_COMMON_TEST_FILES}
    ${DC_CORE_TEST_FILES}
    ${DC_WEBSERVICE_TEST_FILES}
  )
endif()

# Recursively compile unit tests for *.cpp files in the current folder,
# linking with TEST_LIBRARIES and excluding EXCLUDE_FROM_TESTS
include(CommonCTest)
