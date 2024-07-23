CFLAGS = -g -Wall -pedantic 

# VPATH = src/ethernet-study
# DIR = cs431/ethernet-study




all: hexdump hexread hosts stack stack2
.Phony: all

hosts: host1 host2 host3
.Phony: $^

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

#(DIR)/%.o: %.c
#	gcc $(CFLAGS) -c -o $@ $^



stack: stack.o util.o cs431vde.o crc32.o ethernet.o ip.o arp.o tcp1.o
	gcc $(CFLAGS) -o stack stack.o util.o cs431vde.o crc32.o ethernet.o ip.o arp.o tcp1.o

hexdump: hexdump.o util.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o

hexread: hexread.o util.o
	gcc $(CFLAGS) -o hexread hexread.o util.o


host1: host1.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o host1 host1.o util.o cs431vde.o crc32.o

host2: host2.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o host2 host2.o util.o cs431vde.o crc32.o

host3: host3.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o host3 host3.o util.o cs431vde.o crc32.o


receiver: receiver.o util.o cs431vde.o
	gcc $(CFLAGS) -o receiver receiver.o util.o cs431vde.o


stack2: stack2.o util.o cs431vde.o crc32.o ethernet.o ip.o arp.o tcp1.o
	gcc $(CFLAGS) -o stack2 stack2.o util.o cs431vde.o crc32.o ethernet.o ip.o arp.o tcp1.o



.Phony: clean
clean:
	rm -f util hexdump util.o hexdump.o hexread hexread.o stack stack.o sender sender.o receiver receiver.o crc32.o cs431vde.o host1.o host1 host2.o host host1.o host1 host2.o host2 stack2 stack2.o
