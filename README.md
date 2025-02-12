# python plugins for MADS

This is a collection of plugins for [MADS](https://github.com/MADS-NET/MADS). The project creates the plugins `python_source.plugin`, `python_filter.plugin`, and `python_sink.plugin`.

The plugins provide an interface to [python](https://python.org) scripts, allowing you to run python scripts from MADS.

*Required MADS version: 1.0.3.*


## Supported platforms

Currently, the supported platforms are:

* **Linux** 
* **MacOS**
* **Windows**

**Note**: you also need to have NumPy installed.

**Note**: On windows, I have only tested the situation when Python 3.12 has been installed by checking the install option **install for all computer users**.


## Installation

Linux and MacOS:

```bash
cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$(mads -p)"
cmake --build build -j4
sudo cmake --install build
```

Note that you neeed to have installed both Python and NumPy.


Windows:

```powershell
cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$(mads -p)"
cmake --build build --config Release
cmake --install build --config Release
```


## INI settings

The plugin supports the following settings in the INI file:

```ini
[python_source]
venv = "/path/to/venv"
python_module = "source"
search_paths = []

[python_filter]
venv = "/path/to/venv"
python_module = "filter"
search_paths = []

[python_sink]
venv = "/path/to/venv"
python_module = "sink"
search_paths = []
```

The `python_module` is python script file to load. The module name correspondo to a `[module].py` file that is searched in the following directories, in this order:

1. the folder `./python` relative to the folder containing the current executable file
2. the folder `./scripts` relative to the folder containing the current executable file
3. the folder `../python` relative to the folder containing the current executable file
4. the folder `../scripts` relative to the folder containing the current executable file
5. the absolute path `<INSTALL_PREFIX>/python`
6. the absolute path `<INSTALL_PREFIX>/scripts`
7. any other path in the `search_path` list, which bust be either absolute or relative to the executable file

Note that the **executable file is typically the MADS plugin loader**.

All settings are optional; if omitted, the default values are used. Note, however, that the section `[python_xxx]` is mandatory (and can be empty).


## python details

The python module script has the package search list set to the standard search paths, plus the same directories above detailed, where python scripts are loaded. If you need an additional library or scripts, just put it in one of those dirs and require it.

The python script module implicitly has access to the global variables `data` and `topic`, which are the payload and topic, respectively. It **must implement** the following functions, depending on the plugin type:

### Source plugin (`python_source.plugin`)

The python script **must implement** the function `get_output()`, which is called by the plugin obtaining a new payload. The function must return a JSON-formatted string (use `json.dumps(<new_data>)`).

### Filter plugin (`python_filter.plugin`)

The python script **must implement** the function `process()`, which is called by the plugin for each payload. The function must return a JSON-formatted string (use `json.dumps(<filtered_data>)`), and has access to the `data` dictionary and the `topic` string.

### Sink plugin (`python_sink.plugin`)

The python script **must implement** the function `deal_with_data()`, which is called by the plugin for each payload. The function must return nothing and can access the dictionary `data` and the string `topic`.

## Python virtual environment

If you need to use third party Python libraries installed with pip in a virtual environment, you can pass the path to the `venv` folder as the `venv` setting in the INI file. The plugin will activate the virtual environment before running the Python script, so that you can use the libraries installed in the virtual environment with the usual `import` statement.

The test executables automatically detect the `venv` path from the `VIRTUAL_ENV` envoronment variable and set the `venv` setting accordingly. In turn , `VIRTUAL_ENV` is usually set by executing the `activate` script in the virtual environment folder.

---