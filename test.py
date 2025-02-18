#!/usr/bin/env python3

import serial
import json

def read_serial(port, baudrate):
  try:
    ser = serial.Serial(port, baudrate)
    while True:
      line = ser.readline().decode('utf-8').strip()
      print(line)
  except serial.SerialException as e:
    print(f"Error: {e}")
  except KeyboardInterrupt:
    print("Exiting...")
  finally:
    if ser.is_open:
      ser.close()

if __name__ == "__main__":
  port = '/dev/cu.usbmodem34B7DA5F9A5C2'  # Change this to your serial port
  baudrate = 115200  # Change this to your baud rate
  read_serial(port, baudrate)