FROM archlinux:base-devel-20230604.0.155602

RUN pacman -Syu --noconfirm && pacman -S base-devel wget git gdb vim svn debuginfod netcat --noconfirm
RUN wget https://yx7.cc/code/ynetd/ynetd-0.1.2.tar.xz -O /ynetd-0.1.2.tar.xz
RUN tar -xf ynetd-0.1.2.tar.xz
RUN make -C /ynetd-0.1.2/

RUN ln /usr/sbin/nc /usr/bin/ncat

#COPY install-glibc-debug.sh /root/install-glibc-debug.sh
#RUN chmod +x /root/install-glibc-debug.sh
#RUN /root/install-glibc-debug.sh
COPY bashrc /root/.bashrc

## Add your own dummy get_flag here
COPY get_flag /bin/get_flag
COPY gdb-pwndbg /usr/bin/gdb-pwndbg
COPY gdb-gef /usr/bin/gdb-gef
COPY gdb-peda /usr/bin/gdb-peda
COPY gdbinit /root/.gdbinit
COPY vulndbg /usr/bin/vulndbg

RUN useradd -m pwn

RUN git clone https://github.com/pwndbg/pwndbg && cd pwndbg && ./setup.sh && cd .. && mv pwndbg ~/pwndbg-src && echo "source ~/pwndbg-src/gdbinit.py" > ~/.gdbinit_pwndbg
RUN git clone https://github.com/longld/peda.git ~/peda
#RUN wget -q -O /root/.gdbinit-gef.py https://github.com/hugsy/gef/raw/master/gef.py
COPY gef.py /root/.gdbinit-gef.py

#COPY dockerhub_libc-2.37-3.so /usr/lib/libc.so 
#RUN ln -sf /usr/lib/libc.so /usr/lib/libc.so.6
#COPY dockerhub_libc-2.37-3.so /lib/x86_64-linux-gnu/libc.so 
#RUN ln -sf /lib/x86_64-linux-gnu/libc.so /lib/x86_64-linux-gnu/libc.so.6

COPY vuln /home/pwn/vuln
COPY vuln.c /home/pwn/vuln.c

RUN chmod 0755 /home/pwn/vuln

EXPOSE 1339

CMD ["/ynetd-0.1.2/ynetd", "-p", "1339", "-u", "pwn", "-d", "/home/pwn", "./vuln"]

