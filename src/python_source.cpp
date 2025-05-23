/*
  ____                                   _             _
 / ___|  ___  _   _ _ __ ___ ___   _ __ | |_   _  __ _(_)_ __
 \___ \ / _ \| | | | '__/ __/ _ \ | '_ \| | | | |/ _` | | '_ \
  ___) | (_) | |_| | | | (_|  __/ | |_) | | |_| | (_| | | | | |
 |____/ \___/ \__,_|_|  \___\___| | .__/|_|\__,_|\__, |_|_| |_|
                                  |_|            |___/
# A Template for PythonSourcePlugin, a Source Plugin
# Generated by the command: plugin -t source -d python_plugin python_source
# Hostname: Fram-IV.local
# Current working directory: /Users/p4010/Develop/MADS_plugins
# Creation date: 2024-08-30T09:11:04.862+0200
# NOTICE: MADS Version 1.2.0
*/
// Mandatory included headers
#include <nlohmann/json.hpp>
#include <pugg/Kernel.h>
#include <source.hpp>
// other includes as needed here
#include "common.hpp"


// Define the name of the plugin
#ifndef PLUGIN_NAME
#define PLUGIN_NAME "python_source"
#endif

// Load the namespaces
using namespace std;
using json = nlohmann::json;

// Plugin class. This shall be the only part that needs to be modified,
// implementing the actual functionality
class PythonSourcePlugin : public Source<json>, public MADS::PythonPlugin {

public:
  // Typically, no need to change this
  string kind() override { return PLUGIN_NAME; }


  return_type get_output(json &out,
                         vector<unsigned char> *blob = nullptr) override {
    out.clear();

    if (!_agent_id.empty())
      out["agent_id"] = _agent_id;
    
    cppy3::Var result;
    try {
      result = cppy3::eval("mads.get_output()");
      // cout << "Result: " << cppy3::WideToUTF8(result.toString()) << endl;
      out = json::parse(result.toString());
    } catch (cppy3::PythonException &e) {
      cerr << "[Python] Error processing data: " << e.what();
      return return_type::error;
    } catch (nlohmann::json::exception &e) {
      cerr << "[Python] Error parsing result: " << cppy3::WideToUTF8(result.toString())
           << endl
           << e.what();
      return return_type::error;
    }

    return return_type::success;
  }

  void set_params(void const *params) override {
    Source::set_params(params);
    _params["python_module"] = "source";
    _params["search_paths"] = json::array();
    _params["venv"] = "";
    _params.merge_patch(*(json *)params);
    for (auto &path : _params["search_paths"]) {
      _default_paths.push_back(path.get<string>());
    }
    _python_module = _params["python_module"].get<string>();
    
    try {
      setup_venv(_params);
    } catch (exception &e) {
      cerr << "[Python] Error preparing Python: " << e.what();
      exit(EXIT_FAILURE);
    }
    prepare_python(_params);
  }


  map<string, string> info() override {
    return {{"module", _python_module}};
  };

private:
};

/*
  ____  _             _             _      _
 |  _ \| |_   _  __ _(_)_ __     __| |_ __(_)_   _____ _ __
 | |_) | | | | |/ _` | | '_ \   / _` | '__| \ \ / / _ \ '__|
 |  __/| | |_| | (_| | | | | | | (_| | |  | |\ V /  __/ |
 |_|   |_|\__,_|\__, |_|_| |_|  \__,_|_|  |_| \_/ \___|_|
                |___/
Enable the class as plugin
*/
INSTALL_SOURCE_DRIVER(PythonSourcePlugin, json)

/*
                  _
  _ __ ___   __ _(_)_ __
 | '_ ` _ \ / _` | | '_ \
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|

For testing purposes, when directly executing the plugin
*/

#include <cstdlib>

#ifdef ENABLE_SERIALPORT

int main(int argc, char const *argv[]) {
  PythonSourcePlugin plugin;
  json output, params;

  params["python_module"] = "serial_in";
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <serial_port>" << endl;
    return 1;
  }
  params["port"] = argv[1];
  params["baudrate"] = 115200;

  // Set the parameters
  plugin.set_params(&params);

  while (true) {
    plugin.get_output(output);
    cout << "[C++] Output: " << output.dump(2) << endl;
  }

  return 0;
}

#else

int main(int argc, char const *argv[]) {
  PythonSourcePlugin plugin;
  json output, params;

  params["python_module"] = "source";
  if (argc > 1) {
    params["python_module"] = argv[1];
  }
  // Set the parameters
  plugin.set_params(&params);

  // Process data
  plugin.get_output(output);

  // Produce output
  cout << "Output: " << output << endl;

  return 0;
}

#endif