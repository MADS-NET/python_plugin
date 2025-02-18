import json
import serial

# This is an optional function that is called once, when the script is loaded.
# Use it for exampleto open a serial port connection.
# use the dictionary 'state' to store state objects for this script (e.g. port)
def setup():
  print("[Python] Setting up filter...")
  print("[Python] Parameters: " + json.dumps(params))
  state["sp"] = serial.Serial(params["port"], params["baudrate"])


# This is a mandatory function that must be implemented in the script.
# The function must return a JSON string.
def get_output():
  line = state["sp"].readline().decode('utf-8').strip()
  try:
    line = json.loads(line)
  except json.JSONDecodeError:
    line = {"error": "Invalid JSON"}
  return json.dumps(line)
 