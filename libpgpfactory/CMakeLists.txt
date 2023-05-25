include(FeatureSummary)

# LibGpgError bug must include_directories on root, but can be linked in subdirectory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/gpgfactory/cmake)
find_package(LibGpgError REQUIRED)
include_directories(${LibGpgError_INCLUDE_DIRS})
# End LibGpgError

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
if(WIN32 OR MSVC OR MSYS OR MINGW)
    set(WINDOWS TRUE)
endif()


find_package(Gpgmepp CONFIG REQUIRED)




include_directories(${GPGMEAPP_INCLUDES})




set(libpgpfactory_SRCS
    libpgpfactory.h
    libpgpfactory.cpp
    libpasshelper.h
    libpasshelper.cpp   
    FileSearch.cpp
    FileSearch.h
    GpgIdManage.cpp
    GpgIdManage.h
    RunShellCmd.cpp
    RunShellCmd.h
    uuid.cpp
    uuid.h
    PassFile.h
    PassFile.cpp
    WatchWaitAndNoneWaitRunCmdItem.h 
    WatchWaitAndNoneWaitRunCmdItem.cpp
    WatchWaitAndNoneWaitRunCmd.h 
    WatchWaitAndNoneWaitRunCmd.cpp
    # hygen srcs 
)

# Declare the library
add_library(libpgpfactory STATIC
    ${libpgpfactory_SRCS}
)




target_link_libraries(libpgpfactory  Gpgmepp  ${LibGpgError_LIBRARIES})






# Specify here the include directories exported
# by this library
target_include_directories(libpgpfactory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)




set_target_properties(libpgpfactory PROPERTIES LINKER_LANGUAGE CXX)
