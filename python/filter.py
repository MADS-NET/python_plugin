import sys
import json

class MADS(MADS_base):

  @classmethod
  def process(cls):
    print("Processing data...")
    data = MADS.data
    print(data)
    return json.dumps(data)

print("*** TESTING ***")
str = '{"name": "MADS"}'
MADS.load_data(str)
print("Data: ", MADS.data)
print("*** END OF TESTING ***")