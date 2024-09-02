import json

# This is a mandatory function that must be implemented in the script.
# The function must return a JSON string.
def get_output():
  data = {"list": [1, 2, 3, 4]}
  data["processed"] = True
  return json.dumps(data)
