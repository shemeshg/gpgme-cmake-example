cmake_minimum_required(VERSION 3.0.0)
set (CMAKE_CXX_STANDARD 17)
project(pass-cpp VERSION 0.1.0)


include(CTest)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LibGpgError REQUIRED)
include_directories(${LibGpgError_INCLUDE_DIRS})


add_subdirectory(libpgpfactory)


add_executable(pass-cpp main.cpp)
target_link_libraries(pass-cpp libpgpfactory )


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

