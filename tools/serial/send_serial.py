#! /usr/bin/env python3
import serial
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="The port to send the data to")
    parser.add_argument("--data", "-d", help="The data to send", default="0x11")
    args = parser.parse_args()
    ser = serial.Serial(args.port, 115200)
    ser.write(int(args.data, 0) .to_bytes(byteorder='big'))
    print(bin(int(args.data, 0)), "sent")
    ser.close()
