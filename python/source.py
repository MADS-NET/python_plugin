import json

def get_output():
  data = {"list": [1, 2, 3, 4]}
  data["processed"] = True
  return json.dumps(data)
