#include <iostream>

#include <pybind11/embed.h> 
namespace py = pybind11;

#include "libpasshelper.h"
#include "GpgIdManage.h"
#include "RunShellCmd.h"
#include "FileSearch.h"

class PassSimpleBal {
public:
    PassHelper ph{};
    void listKeys(std::string pattern="", bool secret_only=false){
        for (auto r : ph.listKeys(pattern, secret_only))
        {
            std::cout << "we have " << r.getKeyStr() << "\n";
        }  
    }
};

PYBIND11_EMBEDDED_MODULE(pass_simple, m) {
    py::class_<PassSimpleBal>(m, "PassSimpleBal")        
        .def(py::init<>())
        .def("listKeys", &PassSimpleBal::listKeys,"list keys",
             py::arg("pattern") = "", py::arg("secret_only") = false);            
}

int main(int, char **)
{   
    
    py::scoped_interpreter guard{}; //start pybind11
    py::exec(R"(
        import pass_simple
        c=pass_simple.PassSimpleBal()
        c.listKeys()
    )");
    for (std::string line; std::getline(std::cin, line);) {
        py::exec(line);
    }

}
