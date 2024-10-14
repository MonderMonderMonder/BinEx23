FROM debian:bullseye

RUN apt update -y && apt upgrade -y && apt install -y build-essential wget

RUN wget https://yx7.cc/code/ynetd/ynetd-0.1.2.tar.xz -O /ynetd-0.1.2.tar.xz

RUN tar -xf ynetd-0.1.2.tar.xz

RUN make -C /ynetd-0.1.2/

## Add your own dummy get_flag here
COPY get_flag /bin/get_flag

RUN useradd -m pwn

COPY vuln /home/pwn/vuln

RUN chmod 0755 /home/pwn/vuln

EXPOSE 1337

CMD ["/ynetd-0.1.2/ynetd", "-p", "1337", "-u", "pwn", "-d", "/home/pwn", "./vuln"]
