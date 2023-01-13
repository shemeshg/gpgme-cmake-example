cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 17)
project(pass-cpp VERSION 0.1.0)


include(CTest)
enable_testing()

# LibGpgError bug must include_directories on root, but can be linked in subdirectory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(LibGpgError REQUIRED)
include_directories(${LibGpgError_INCLUDE_DIRS})
# End LibGpgError 

add_subdirectory(libpgpfactory)
add_subdirectory(main)



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

