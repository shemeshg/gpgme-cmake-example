cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 17)
project(pass-cpp VERSION 0.1.0)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
if(WIN32 OR MSVC OR MSYS OR MINGW)
    set(WINDOWS TRUE)
endif()

if(MACOSX)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" )
    include( "/Users/macos/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()
if(LINUX)
    include( "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()
if(WINDOWS)
include( "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

include(CTest)
enable_testing()

if(UNIX)
    # LibGpgError bug must include_directories on root, but can be linked in subdirectory
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/libpgpfactory/cmake)
    find_package(LibGpgError REQUIRED)
    include_directories(${LibGpgError_INCLUDE_DIRS})
    # End LibGpgError 
endif()

add_subdirectory(libpgpfactory)
add_subdirectory(main)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

