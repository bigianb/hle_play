cmake_minimum_required(VERSION 3.9)

# macOS deployment target needs to be set before 'project' to work
if(APPLE AND NOT TARGET_IOS)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
endif()

project(HLEPlay)

# Use this to find Qt on windows
if(ADDITIONAL_FIND_PATH)
    list(APPEND CMAKE_PREFIX_PATH ${ADDITIONAL_FIND_PATH})
endif()

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/Play/deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

set(PROJECT_NAME "HLE Play")
set(PROJECT_Version 0.10)

execute_process(COMMAND git describe RESULT_VARIABLE GIT_DESCRIBE_RESULT OUTPUT_VARIABLE GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_DESCRIBE_RESULT EQUAL 0)
	message("Building for ${GIT_TAG}.")
	set(PROJECT_Version ${GIT_TAG})
else()
	message("Warning: Failed to obtain Git tag.")
endif()

add_definitions(-DPLAY_VERSION="${PROJECT_Version}")
set(PROJECT_LIBS)

include(PrecompiledHeader)
add_subdirectory(Source)




