cmake_minimum_required(VERSION 3.15)
project(RnpCore VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(UNIX)
    include(FetchContent)

    FetchContent_Declare(
        rnp
        GIT_REPOSITORY https://github.com/rnpgp/rnp.git
        GIT_TAG v0.18.1
    )
FetchContent_GetProperties(rnp)

if(NOT rnp_POPULATED)
    message(STATUS "Fetching rnp...")
    FetchContent_MakeAvailable(rnp)
endif()
endif()


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

if(WIN32 OR MSVC OR MSYS OR MINGW)
    if(CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
        set(RNP_REPOSITORY_DIR "C:/Users/windows/Documents/Projects/rnp")
        set(RNP_BUILD_SRC_DIR "${RNP_REPOSITORY_DIR}/build/src")
        set(RNP_VCPKG_LIB_DIR "${RNP_REPOSITORY_DIR}/vcpkg_install/x64-windows")

        ## WINDOWS ONLY rnp
        add_library(librnp STATIC IMPORTED)
        set_target_properties(librnp PROPERTIES
            IMPORTED_LOCATION "${RNP_BUILD_SRC_DIR}/lib/Release/rnp.lib"
            INTERFACE_INCLUDE_DIRECTORIES
            "${RNP_REPOSITORY_DIR}/include;${RNP_BUILD_SRC_DIR}/lib"
        )

        add_library(sexpplib STATIC IMPORTED)
        set_target_properties(sexpplib PROPERTIES
            IMPORTED_LOCATION "${RNP_BUILD_SRC_DIR}/libsexpp/Release/sexpp.lib"
        )

        add_library(libBotan STATIC IMPORTED)
        set_target_properties(libBotan PROPERTIES
            IMPORTED_LOCATION "${RNP_VCPKG_LIB_DIR}/lib/botan-3.lib"
        )

        add_library(nz2lib STATIC IMPORTED)
        set_target_properties(nz2lib PROPERTIES
            IMPORTED_LOCATION "${RNP_VCPKG_LIB_DIR}/lib/z.lib"
        )

        add_library(bz2lib STATIC IMPORTED)
        set_target_properties(bz2lib PROPERTIES
            IMPORTED_LOCATION "${RNP_VCPKG_LIB_DIR}/lib/bz2.lib"
        )

        add_library(jsonclib STATIC IMPORTED)
        set_target_properties(jsonclib PROPERTIES
            IMPORTED_LOCATION "${RNP_VCPKG_LIB_DIR}/lib/json-c.lib"
        )

        add_library(getoptlib STATIC IMPORTED)
        set_target_properties(getoptlib PROPERTIES
            IMPORTED_LOCATION "${RNP_VCPKG_LIB_DIR}/lib/getopt.lib"
        )

        target_link_libraries(RnpCore PRIVATE libBotan nz2lib jsonclib getoptlib bz2lib sexpplib)
    else()
        # arm64
    endif()
endif()

target_link_libraries(RnpCore PRIVATE librnp)

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
