#! /usr/bin/env python3
import serial
import sys

if __name__ == "__main__":
    ser = serial.Serial(sys.argv[1], 115200)
    ser.rts = False
    ser.dtr = False
    ser.open()
    ser.write(b"\x11")
    ser.close()
