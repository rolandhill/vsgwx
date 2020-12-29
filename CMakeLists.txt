cmake_minimum_required(VERSION 3.7)

project(wxsi
        VERSION 0.0.0
        DESCRIPTION "wxWidgets tests for SI"
        LANGUAGES CXX
)

# build all examples into the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Change the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

FIND_PACKAGE ( PkgConfig REQUIRED )
PKG_CHECK_MODULES( GTK REQUIRED gtk+-3.0 ) # or use gtk+-2.0, etc...

#set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

# find the vsg
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg REQUIRED)

# find the optional vsgXchange that can be used for reading and range of image and 3d model formats and shader compilation
find_package(vsgXchange QUIET)

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)

# add clobber build target to clear all the non git registered files/directories
add_custom_target(clobber
    COMMAND git clean -d -f -x
)

set(SOURCES
    src/si.cpp
    src/wxVsgWindow.cpp
)

add_executable(wxsi ${SOURCES})

target_include_directories( wxsi PUBLIC
    include
    ${GTK_INCLUDE_DIRS}
)

target_link_libraries(wxsi vsg::vsg ${wxWidgets_LIBRARIES} ${GTK_LIBRARIES})

if (vsgXchange_FOUND)
    target_compile_definitions(wxsi PRIVATE USE_VSGXCHANGE)
    target_link_libraries(wxsi vsgXchange::vsgXchange)
endif()

install(TARGETS wxsi
        RUNTIME DESTINATION bin
)

