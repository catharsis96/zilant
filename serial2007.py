import serial
import sys
from time import sleep
import json
import urllib
import re
import requests

try:
  ser = serial.Serial("COM6", 115200,timeout=0, parity=serial.PARITY_NONE, 
                        stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
except:
  sys.exit("Error connecting device")

ls = b"001"
rs = b"002"
buffer = b""
while True:
  buffer_string = ser.readline(ser.inWaiting())
  if b"001" in buffer_string and b"002" not in buffer_string:
    byFirstBracket = buffer_string.split(b"001")
    buffer += byFirstBracket[1]
    continue
  elif b"001" in buffer_string and b"002" in buffer_string:
    buffer += re.search('%s(.*)%s' % ("001", "002"), buffer_string).group(1)
    continue
  elif b"001" not in buffer_string and b"002" in buffer_string:
    buffer += buffer_string.split(b"002")[0]
  elif b"001" not in buffer_string and b"002" not in buffer_string and buffer_string:
    buffer += buffer_string
    continue
  else:
    continue
  try:
    data = json.loads(buffer.decode('utf-8'))
    url = "http://137.117.173.220:3000/sendSensorData"
    #data = {'sender': 'Alice', 'receiver': 'Bob', 'message': 'We did it!'}
    headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
    requests.post(url, data=json.dumps(data), headers=headers)
  except Exception as e:
    print("EXC", e)
  sleep(0.2)
  buffer = b""