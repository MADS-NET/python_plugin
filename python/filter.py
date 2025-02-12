import sys


# This is a mandatory function that must be implemented in the script.
# The function must return a JSON string.
def process():
  print("[Python] Parameters: " + json.dumps(params))
  print("[Python] Processing data from topic '" + topic + "'...")
  data["processed"] = True
  return json.dumps(data)
