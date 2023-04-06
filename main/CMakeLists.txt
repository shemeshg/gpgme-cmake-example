find_package(pybind11 REQUIRED)

add_executable(pass-cpp main.cpp)
target_link_libraries(pass-cpp libpgpfactory)
target_link_libraries(pass-cpp pybind11::embed)
