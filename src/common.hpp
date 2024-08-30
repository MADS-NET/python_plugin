
#include <string>
#include <vector>
#include <cppy3/cppy3.hpp>
#include <cppy3/utils.hpp>

#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#warning "INSTALL_PREFIX not defined, using /usr/local"
#endif

namespace MADS {

class PythonPlugin {
public:
  void prepare_python() {
    cppy3::exec("import sys");
    for (auto &path : _default_paths) {
      cppy3::exec("sys.path.append('" + path + "')");
    }
    try {
      cppy3::exec("import " + _python_module + " as mads");
    } catch (cppy3::PythonException &e) {
      std::cerr << "Error loading module: " << e.what();
      exit(EXIT_FAILURE);
    }
    cppy3::exec(R"(
mads.data = {}
mads.topic = ''
    )");
  }

protected:
  cppy3::PythonVM _python;
  std::string _python_module;
  // clang-format off
  std::vector<std::string> _default_paths = {
      "./python",      
      "./scripts",           
      "../python",
      "../scripts", 
      "../../python",
      "../../scripts", 
      INSTALL_PREFIX "/python", 
      INSTALL_PREFIX "/scripts"};
  // clang-format on
};

} // namespace MADS