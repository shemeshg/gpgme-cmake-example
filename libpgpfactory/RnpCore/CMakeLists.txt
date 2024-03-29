cmake_minimum_required(VERSION 3.15)
project(RnpCore VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)

pkg_check_modules(JSONC IMPORTED_TARGET json-c12)
if(NOT JSONC_FOUND)
    pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c)
endif(NOT JSONC_FOUND)

add_library(JSON-C::JSON-C INTERFACE IMPORTED)
set_target_properties(JSON-C::JSON-C PROPERTIES INTERFACE_LINK_LIBRARIES PkgConfig::JSONC)

pkg_check_modules(Botan REQUIRED IMPORTED_TARGET botan-3)
add_library(Botan::Botan  INTERFACE IMPORTED)
set_target_properties(Botan::Botan PROPERTIES INTERFACE_LINK_LIBRARIES PkgConfig::Botan)

find_package(rnp REQUIRED)


add_library(RnpCore 

RnpCoreDefinitions.h

rnpcpp.hpp
RnpCoreParams.h 
RnpCoreParams.cpp 
RnpCoreBal.h 
RnpCoreBal.cpp 
FfiRaII.h
FfiRaII.cpp
export/RnpCoreInterface.h
export/RnpKeys.h
export/RnpLoginRequestException.h
RnpCoreInterface.cpp
)

target_link_libraries(RnpCore PRIVATE rnp::librnp JSON-C::JSON-C)

target_include_directories(RnpCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/export
)

include(GNUInstallDirs)
install(TARGETS RnpCore
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT cli
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
