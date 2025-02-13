/*
   ____                                      
  / ___|___  _ __ ___  _ __ ___   ___  _ __  
 | |   / _ \| '_ ` _ \| '_ ` _ \ / _ \| '_ \ 
 | |__| (_) | | | | | | | | | | | (_) | | | |
  \____\___/|_| |_| |_|_| |_| |_|\___/|_| |_|
                                             
Common header for source, filter and sink python plugins
*/
#include <string>
#include <vector>
#include <cppy3/cppy3.hpp>
#include <cppy3/utils.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>


#ifndef INSTALL_PREFIX
#define INSTALL_PREFIX "/usr/local"
#warning "INSTALL_PREFIX not defined, using /usr/local"
#endif

using namespace std::filesystem;
using json = nlohmann::json;

namespace MADS {

class PythonPlugin {
public:
  void prepare_python(json const &params) {
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
import json
mads.data = {}
mads.state = {}
mads.topic = ''
    )");
    cppy3::exec("mads.params = json.loads('" + params.dump() + "')");
    try {
      cppy3::exec("mads.setup()");
    } catch (cppy3::PythonException &e) {
      std::cerr << "[Python] running setup(): " << e.what();
    }
  }

  void setup_venv(json const &params) {
    if (params["venv"].is_null() || params["venv"].get<std::string>().empty()) {
      std::cerr << "[Python] No virtual environment specified, using system Python libraries" << std::endl;
    } else if (!exists(params["venv"].get<path>())) {
      throw std::runtime_error("Virtual environment does not exist: " + params["venv"].get<std::string>());
    } else {
      path venv = params["venv"].get<path>();
      path lib = venv / "lib";
      path site_packages;
      for (auto &entry : directory_iterator(lib)) {
        if (entry.is_directory() && entry.path().filename().string().find("python") != std::string::npos) {
          site_packages = entry.path() / "site-packages";
          break;
        }
      }
      if (site_packages.empty()) {
        throw std::runtime_error("Could not find site-packages in virtual environment: " + venv.string());
      }
      cppy3::exec("import site");
      cppy3::exec("site.addsitedir('" + site_packages.string() + "')");
    } 
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