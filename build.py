#!/usr/bin/python3
import sys
import os

def main():
    os.system('cd code && mkdir -p build && cd build && cmake ..')
    os.system('cd code/build && make -j4')
    command='cd code/build && ./Rasterizer'
    for argv in sys.argv[1:]:
        command += ' ' + argv + ' '
    os.system(command)

if __name__ == "__main__":
    main()