#! /usr/bin/env python3

import subprocess
import time
import signal
import os


class VirtualSerial:
    def __init__(self) -> None:
        self.index = 0
        self.virtual_filename = "./vPTY"
        self.ps = []
        self.used_filename = []

    def connect_virtual_serial(self, baudrate: int = 57600) -> None:
        self.__check_filename(f"{self.virtual_filename}{self.index}")
        self.__check_filename(f"{self.virtual_filename}{self.index + 1}")
        # Connect to the virtual serial port
        option = f"pty,raw,echo=0,ispeed={baudrate},ospeed={baudrate},link={self.virtual_filename}{self.index}"
        option2 = f"pty,raw,echo=0,ispeed={baudrate},ospeed={baudrate},link={self.virtual_filename}{self.index + 1}"
        print(f"\n{self.virtual_filename}{self.index} <-> {self.virtual_filename}{self.index + 1}")
        print(f"Baudrate: {baudrate}")
        self.ps.append(subprocess.Popen(["socat", "-d", "-d", option, option2]))
        self.used_filename.append(f"{self.virtual_filename}{self.index}")
        self.used_filename.append(f"{self.virtual_filename}{self.index + 1}")
        self.index += 2

    def connect_virtual_serial_raw(self, baudrate: int, filename: str) -> None:
        self.__check_filename(f"{filename}0")
        self.__check_filename(f"{filename}1")
        # Connect to the virtual serial port
        option = f"pty,raw,echo=0,ispeed={baudrate},ospeed={baudrate},link={filename}0"
        option2 = f"pty,raw,echo=0,ispeed={baudrate},ospeed={baudrate},link={filename}1"
        print(f"\n{filename}0 <-> {filename}1")
        print(f"Baudrate: {baudrate}")
        self.index += 2
        self.ps.append(subprocess.Popen(["socat", "-d", "-d", option, option2]))
        self.used_filename.append(f"{filename}0")
        self.used_filename.append(f"{filename}1")

    def close_virtual_serial(self) -> None:
        for p in self.ps:
            p.send_signal(signal.SIGINT)
            while p.poll() is None:
                time.sleep(0.01)

        for path_str in self.used_filename:
            if (os.path.lexists(path_str) and not os.path.exists(path_str)):
                print(f"{path_str} remained broken. Unlink it...")
                os.unlink(path_str)
        self.ps.clear()
        self.index = 0

    def __del__(self) -> None:
        self.close_virtual_serial()

    def __check_filename(self, filename: str) -> None:
        if filename in self.used_filename:
            self.close_virtual_serial()
            raise ValueError(f"{filename} is already used.")

    def connect_virtual_serial_multi(self, baudrates: list[int], filenames: list[str] | None = None) -> None:
        if filenames is not None and len(filenames) != len(baudrates):
            raise ValueError(f"The length of baudrates ({len(baudrates)}) and filenames ({len(filenames)}) must be the same.")
        if (filenames is None):
            for b in baudrates:
                self.connect_virtual_serial(b)
        else:
            for i in range(len(baudrates)):
                self.connect_virtual_serial_raw(baudrates[i], filenames[i])
        print("\nAll virtual serial ports are connected.")
        try:
            while True:
                pass
        except KeyboardInterrupt:
            self.close_virtual_serial()

    @ staticmethod
    def default_setting() -> 'VirtualSerial':
        ret = VirtualSerial()
        ret.connect_virtual_serial_multi([57600, 1200], ["./telemetryPTY", "./commandPTY"])
        return ret


if __name__ == "__main__":
    virtual_serial = VirtualSerial.default_setting()
