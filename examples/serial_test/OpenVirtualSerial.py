#! /usr/bin/env python3

import sys
sys.path.append('../../tools/virtual_serial')
import connect_virtual_serial

if __name__ == "__main__":
    virtual_serial = connect_virtual_serial.VirtualSerial.default_setting()
