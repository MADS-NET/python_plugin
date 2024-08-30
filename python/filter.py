import json

def process():
  print("[Python] Processing data from topic '" + topic + "'...")
  data["processed"] = True
  return json.dumps(data)
