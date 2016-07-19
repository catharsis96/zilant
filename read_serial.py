import serial
import sys
from time import sleep
import json
import urllib
import re

try:
  ser = serial.Serial("COM6", 115200,timeout=0, parity=serial.PARITY_NONE, 
                        stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
except:
  sys.exit("Error connecting device")

buffer = b""
while True:
  buffer_string = ser.readline(ser.inWaiting())
  if b"{" in buffer_string and b"}" not in buffer_string:
    byFirstBracket = buffer_string.split(b"{")
    buffer += b"{" + byFirstBracket[1]
    continue
  elif b"{" in buffer_string and b"}" in buffer_string:
    buffer += b"{"+re.search('%s(.*)%s' % ("{", "}"), buffer_string).group(1)+b"}"
    continue
  elif b"{" not in buffer_string and b"}" in buffer_string:
    buffer += buffer_string.split(b"}")[0] + b"}"
  elif b"{" not in buffer_string and b"}" not in buffer_string and buffer_string:
    buffer += buffer_string
    continue
  else:
    continue
  try:
    #print(buffer)
    #data = json.loads(buffer)
    req = urllib.Request(url)
    req.add_header('Content-Type','application/json')
    #data = json.dumps(data)
    response = urllib2.urlopen(req,buffer)
    print("RESPONSE",response)
  except Exception as e:
    print("EXC", e)
    pass
  sleep(0.2)
  buffer = b""