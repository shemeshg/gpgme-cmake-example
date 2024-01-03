cmake_minimum_required(VERSION 3.15)
project(RnpCore VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

find_package(JSON-C  REQUIRED)
if (CRYPTO_BACKEND_BOTAN3)
  find_package(Botan 3.0.0 REQUIRED)
elseif (CRYPTO_BACKEND_BOTAN)
  find_package(Botan 2.14.0 REQUIRED)
  if(BOTAN_VERSION VERSION_GREATER_EQUAL 3.0.0)
    set(CRYPTO_BACKEND_BOTAN3 1)
  endif()
endif()

if(MSVC)
  find_package(WindowsSDK)
  GetUMWindowsSDKLibraryDir(WIN_LIBRARY_DIR)
  message (STATUS "Using Windows SDK library directory: ${WIN_LIBRARY_DIR}")
  find_library(SHLWAPI_LIBRARY
    PATHS
       ${WIN_LIBRARY_DIR}
    NAMES shlwapi)

  find_path(GETOPT_INCLUDE_DIR
    NAMES getopt.h
  )
  find_library(GETOPT_LIBRARY
    NAMES getopt
  )
  find_path(DIRENT_INCLUDE_DIR
    NAMES dirent.h
  )
  target_include_directories(rnp_tests
    PRIVATE
      "${GETOPT_INCLUDE_DIR}"
      "${DIRENT_INCLUDE_DIR}"
    )
  target_link_libraries(rnp_tests
    PRIVATE
      "${SHLWAPI_LIBRARY}"
      "${GETOPT_LIBRARY}"
    )
endif()

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



target_link_libraries(RnpCore PRIVATE rnp
JSON-C::JSON-C
sexpp)

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
