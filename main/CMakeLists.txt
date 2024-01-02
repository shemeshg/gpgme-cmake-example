include( "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
add_executable(pass-cpp main.cpp SetStdinEcho.h)
target_link_libraries(pass-cpp libpgpfactory)
