#! /usr/bin/env python3
import serial
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="The port to send the data to")
    parser.add_argument("--data", "-d", help="The data to send", default=0x11, type=int)
    args = parser.parse_args()
    ser = serial.Serial(args.port, 115200)
    while (True):
        try:
            b = ser.read(1)
            for i in b:
                print(bin(i))
        except KeyboardInterrupt:
            break
    ser.close()
