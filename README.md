# Binary Exploitation CTF Challenges

This repository contains solutions for binary exploitation challenges. Each challenge is numbered with a "pwn" prefix, followed by a two-digit identifier, and includes an explanation of the techniques and vulnerabilities exploited.

## Challenge Descriptions

| Challenge | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| **00** | Exploits a buffer overflow vulnerability that bypasses UID checks and redirects program execution to a hidden function (`win()`), ultimately outputting the flag. Techniques include memory overflow with specific byte alignments to control return addresses. |
| **01** | Leverages a hash collision vulnerability in SHA-256 due to early null byte termination, bypassing checks. With ASLR enabled, a buffer overflow is exploited to control the return address and execute arbitrary code, pointing to `/bin/get_flag`. |
| **02** | Exploits improper null byte handling in buffer boundaries. A stack-based buffer overflow is used to leak a library address and execute a format string vulnerability, allowing memory exploration for libc address calculation, ultimately redirecting execution. |
| **03** | Utilizes a linked list pointer overwrite through buffer manipulation. With ASLR disabled, a partial overwrite of pointer references is possible, directing memory traversal and reading a static flag location. |
| **04** | Based on a stack format string attack and ROP chain. The stack is explored to leak addresses, calculate libc offsets, and trigger ROP gadgets that enable memory dumping and arbitrary code execution to capture the flag. |
| **05** | Exploits a libc address leak through a double call of `puts`. ROP gadgets are used to reinvoke `get` on dynamic memory, revealing the libc base address and re-targeting execution to obtain the flag. |
| **06** | Based on heap manipulation and arbitrary address overwrites. GOT entry redirection and a custom loop trigger `free()` repeatedly, exploiting integer overflow in malloc. ASLR is disabled, making memory manipulation easier, and system commands are executed for flag retrieval. |
| **07** | Exploits a size miscalculation in memory allocation, allowing an integer overflow. The linked list structure is abused to redirect the `next` pointer, resulting in a memory leak to locate the GOT section for libc offsets and flag retrieval. |
| **08** | A brute-force approach on partial overrides in libc memory layout. The exploit redirects canonicalized filename functions to `system` calls using partial pointer overrides in ASLR-aligned memory regions, giving access to the system shell to obtain flags. |
| **09** | Uses a format string exploit to leak stack memory. By specifying positional arguments in the format string, the stack is traversed to reveal the flag stored in a known stack location. |
| **10** | Uses a format string vulnerability to leak a flag stored in the `.bss` section, which isn't affected by ASLR&#8203;. |
| **11** | Exploits stack-based format string vulnerabilities to overwrite stack addresses, incrementally bypassing memory checks&#8203;. |
| **12** | Exploits stack address manipulations and format string vulnerabilities to bypass return address checks and achieve arbitrary jumps&#8203;. |
| **13** | Leverages stack canary leak and partial stack frame shifts to bypass security checks and retrieve flags&#8203;. |
| **14** | Uses integer overflow in input size handling to exploit buffer concatenation vulnerability and execute ROP chains&#8203;. |
| **15** | Exploits a stack overflow with partially predictable inputs, triggering libc address leaks and ROP chain construction&#8203;. |
| **16** | Exploits stack overflow by tricking buffer allocations and buffer overflows to gain control and retrieve flag contents&#8203;. |
| **17** | Uses shellcode injection and a time-based side channel to exploit memory leaks and achieve arbitrary code execution&#8203;. |
| **18** | Constructs a ROP chain based on a password retrieved bit-by-bit using a time-based side-channel attack&#8203;. |
| **19** | Exploits heap overflow and chunk reallocation to leak libc addresses, overwrite `malloc` hooks, and trigger system commands&#8203;. |
| **20** | Exploits a fastbin attack to place a crafted chunk into a fastbin list, which contains function pointers in libc. The exploit targets `system("/bin/sh")` by overwriting a function pointer in libc&#8203;. |
| **21** | Uses a `House of Force` attack by overflowing a top chunk size field, allowing memory allocations that bypass size restrictions and leak libc addresses&#8203;. |
| **22** | Exploits overlapping chunks to rewrite critical memory areas and hijack control flow. It performs a heap manipulation for partial memory control&#8203;. |
| **23** | Uses heap manipulation and pointer tampering with backward and forward chunk consolidation, allowing heap memory leaks and fake chunks insertion&#8203;. |
| **24** | Leveraging overlapping chunks and lazy symbol resolution, this exploit gains arbitrary read and write to overwrite function pointers in the GOT&#8203;. |
| **25** | Uses heap pointer tampering, demonstrating encrypted pointer dereferencing by leveraging chunk overlaps and encrypted pointers in glibc&#8203;. |
| **26** | Exploits integer overflow and heap layout manipulation for partial heap memory access. This allows leaking heap base address and running arbitrary code&#8203;. |
| **27** | Performs vtable pointer manipulation in C++ to control `vtable` entries and execute arbitrary functions by overriding virtual function calls&#8203;. |

## Prerequisites

- Python 3.x
- `pwntools` library

## Usage

Deploy `pwn<challenge_number>/materials/vuln` using the `Dockerfile` then run the exploit script against the server.

```bash
cd ./pwn<challenge_number>/exploit
python3 pwn_137<challenge_number>.py [host] [port]
```
