set(libpgpfactory_SRCS
    libpgpfactory.h
    libpgpfactory.cpp
)

# Declare the library
add_library(libpgpfactory STATIC
    ${libpgpfactory_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(libpgpfactory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(libpgpfactory PROPERTIES LINKER_LANGUAGE CXX)