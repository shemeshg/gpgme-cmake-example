include(FeatureSummary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0003 NEW)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
if(WIN32 OR MSVC OR MSYS OR MINGW)
    set(WINDOWS TRUE)
endif()

add_subdirectory(RnpCore)

if(UNIX)
    # LibGpgError bug must include_directories on root, but can be linked in subdirectory
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    find_package(LibGpgError REQUIRED)
    include_directories(${LibGpgError_INCLUDE_DIRS})
    # End LibGpgError

    find_package(Gpgmepp CONFIG REQUIRED)




    include_directories(${GPGMEAPP_INCLUDES})
endif()





set(libpgpfactory_SRCS    
    export/InterfacePassHelper.h
    InterfacePassHelper.cpp
    export/InterfacePassFile.h
    InterfacePassFile.cpp
    export/GpgKeys.h
    export/InterfaceWatchWaitAndNoneWaitRunCmd.h    
    InterfaceWatchWaitAndNoneWaitRunCmd.cpp
    export/InterfaceWatchWaitAndNoneWaitRunCmdItem.h
    InterfaceWatchWaitAndNoneWaitRunCmdItem.cpp
    libpgpfactory.h
    libpgpfactory.cpp
    libpasshelper.h
    libpasshelper.cpp   
    FileSearch.cpp
    export/FileSearch.h
    GpgIdManage.cpp
    export/GpgIdManage.h
    RunShellCmd.cpp
    export/RunShellCmd.h
    uuid.cpp
    export/uuid.h
    PassFile.h
    PassFile.cpp
    WatchWaitAndNoneWaitRunCmdItem.h 
    WatchWaitAndNoneWaitRunCmdItem.cpp
    WatchWaitAndNoneWaitRunCmd.h 
    WatchWaitAndNoneWaitRunCmd.cpp
    TmpFileWacher.h 
    TmpFileWacher.cpp 
    RnpHelper.h 
    RnpHelper.cpp
    PassFileRnp.h 
    PassFileRnp.cpp 
    # hygen srcs 
)

# Declare the library
add_library(libpgpfactory STATIC
    ${libpgpfactory_SRCS}
)



set (target_link_list RnpCore)

if(UNIX)
    set (target_link_list ${target_link_list} Gpgmepp  ${LibGpgError_LIBRARIES} )    
endif()

SET(USE_GNUPG_ON_WINDOWS OFF)
if (WINDOWS AND USE_GNUPG_ON_WINDOWS)
    # Create an IMPORTED target for the libgpgmepp6-6 DLL
    add_library(libgpgmepp6-6 SHARED IMPORTED)
    # Set the location of the DLL file
    set_target_properties(libgpgmepp6-6 PROPERTIES
    IMPORTED_LOCATION "C:/Program Files (x86)/Gpg4win/bin_64/libgpgmepp6-6.dll"
    )
    # Set the location of the .imp file
    set_target_properties(libgpgmepp6-6 PROPERTIES
    IMPORTED_IMPLIB "C:/Program Files (x86)/Gpg4win/lib/libgpgme.imp"
    )
    # Set the include directories for the IMPORTED target
    set_target_properties(libgpgmepp6-6 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "C:/Program Files (x86)/Gpg4win/include"
    )

    # Create an IMPORTED target for the libgpg-error6-0 DLL
    add_library(libgpg-error6-0 SHARED IMPORTED)
    # Set the location of the DLL file
    set_target_properties(libgpg-error6-0 PROPERTIES
    IMPORTED_LOCATION "C:/Program Files (x86)/Gpg4win/bin_64/libgpg-error6-0.dll"
    )
    # Set the location of the .imp file
    set_target_properties(libgpg-error6-0 PROPERTIES
    IMPORTED_IMPLIB "C:/Program Files (x86)/Gpg4win/lib/libgpg-error.imp"
    )
    # Set the include directories for the IMPORTED target
    set_target_properties(libgpg-error6-0 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "C:/Program Files (x86)/Gpg4win/include"
    )

    # Link your executable target against the IMPORTED targets
    set (target_link_list ${target_link_list} libgpgmepp6-6 libgpg-error6-0)   
endif()

target_link_libraries(libpgpfactory ${target_link_list})

# Specify here the include directories exported
# by this library
target_include_directories(libpgpfactory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/export
)




set_target_properties(libpgpfactory PROPERTIES LINKER_LANGUAGE CXX)
