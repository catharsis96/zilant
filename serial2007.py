import serial
import sys
from time import sleep
import json
import urllib
import re
import requests
from pdb import set_trace
import io
try:
  ser = serial.Serial("COM6", 115200,timeout=0, parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
except:
  sys.exit("Error connecting device")

ls = b"+"
rs = b"-"
buffer = b""
buffer_data=b""
while True:
  buffer = ser.readline()
  if not buffer:
    continue
  if b'+' in buffer:
    buffer_data = buffer_data+buffer[:buffer.find(b'+')]
  else:
    buffer_data += buffer
  if b'}' in buffer_data:
    try:
      data = json.loads(buffer_data.decode('utf-8'))
      url = "http://137.117.173.220:3000/sendSensorData/{uid}/{Temperature}/{Light}/{Humidity}/{Pressure}".format(**data)
      requests.get(url)
      buffer_data=b""
      print("DONE")
    except Exception as e:
      print("EXC", e)
      buffer_data=b""
  sleep(1)
  buffer = b""