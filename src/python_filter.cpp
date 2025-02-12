/*
  _____ _ _ _                    _             _
 |  ___(_) | |_ ___ _ __   _ __ | |_   _  __ _(_)_ __
 | |_  | | | __/ _ \ '__| | '_ \| | | | |/ _` | | '_ \
 |  _| | | | ||  __/ |    | |_) | | |_| | (_| | | | | |
 |_|   |_|_|\__\___|_|    | .__/|_|\__,_|\__, |_|_| |_|
                          |_|            |___/
# A Template for PythonFilterPlugin, a Filter Plugin
# Generated by the command: plugin -t filter -d python_plugin python_filter
# Hostname: Fram-IV.local
# Current working directory: /Users/p4010/Develop/MADS_plugins
# Creation date: 2024-08-30T09:10:53.294+0200
# NOTICE: MADS Version 1.2.0
*/
// Mandatory included headers
#include <filter.hpp>
#include <nlohmann/json.hpp>
#include <pugg/Kernel.h>
// other includes as needed here
#include "common.hpp"

// Define the name of the plugin
#ifndef PLUGIN_NAME
#define PLUGIN_NAME "python_filter"
#endif

// Load the namespaces
using namespace std;
using json = nlohmann::json;

// Plugin class. This shall be the only part that needs to be modified,
// implementing the actual functionality
class PythonFilterPlugin : public Filter<json, json>,
                           public MADS::PythonPlugin {

public:
  // Typically, no need to change this
  string kind() override { return PLUGIN_NAME; }

  return_type load_data(json const &input, string topic = "") override {
    string const load_topic = "mads.topic = '" + topic + "'";
    string const load_data = "mads.data = " + input.dump();

    try {
      cppy3::exec(load_topic);
      cppy3::exec(load_data);
    } catch (cppy3::PythonException &e) {
      cerr << "[Python] Error loading data: " << e.what();
      return return_type::error;
    }
    return return_type::success;
  }

  return_type process(json &out) override {
    out.clear();

    if (!_agent_id.empty())
      out["agent_id"] = _agent_id;

    cppy3::Var result;
    try {
      result = cppy3::eval("mads.process()");
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
    Filter::set_params(params);
    _params["python_module"] = "filter";
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
INSTALL_FILTER_DRIVER(PythonFilterPlugin, json, json);

/*
                  _
  _ __ ___   __ _(_)_ __
 | '_ ` _ \ / _` | | '_ \
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|

*/

#include <cstdlib>

int main(int argc, char const *argv[]) {
  PythonFilterPlugin plugin;
  json params;
  json input, output;

  // Set example values to params
  char *venv_path = getenv("VIRTUAL_ENV");
  if (strlen(venv_path) > 0) {
    cerr << "Using virtual environment from VIRTUAL_ENV shell var: " << venv_path << endl;
    params["venv"] = venv_path;
  }
  params["python_module"] = "filter";
  if (argc > 1) {
    params["python_module"] = argv[1];
  }

  // Set the parameters
  plugin.set_params(&params);

  // Set input data
  input["data"] = {{"AX", 1}, {"AY", 2}, {"AZ", 3}};
  input["data"]["ary"] = {1, 2, 3};
  input["data"]["string"] = "Hello, World!";

  // Set input data
  plugin.load_data(input, "test_topic");
  cout << "[C++] Input: " << input.dump(2) << endl;

  // Process data
  plugin.process(output);
  cout << "[C++] Output: " << output.dump(2) << endl;

  return 0;
}
