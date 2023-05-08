CFLAGS = -g -Wall -pedantic -Wno-pointer-arith

# VPATH = src/ethernet-study
# DIR = cs431/ethernet-study




all: hexdump hexread sender receiver 
.Phony: all

stacksend: stack sender 
.Phony: $^

senders: sender1a sender2a
.Phony: $^

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

#(DIR)/%.o: %.c
#	gcc $(CFLAGS) -c -o $@ $^



stack: stack.o util.o cs431vde.o crc32.o ethernet.o ip.o arp.o  
	gcc $(CFLAGS) -o $@ $^

hexdump: hexdump.o util.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o

hexread: hexread.o util.o
	gcc $(CFLAGS) -o hexread hexread.o util.o

sender: sender.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o sender sender.o util.o cs431vde.o crc32.o

sender1a: sender1a.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o sender1a  sender1a.o util.o cs431vde.o crc32.o

sender2a: sender2a.o util.o cs431vde.o crc32.o
	gcc $(CFLAGS) -o sender2a  sender2a.o util.o cs431vde.o crc32.o


receiver: receiver.o util.o cs431vde.o
	gcc $(CFLAGS) -o receiver receiver.o util.o cs431vde.o


.Phony: clean
clean:
	rm -f util hexdump util.o hexdump.o hexread hexread.o stack stack.o sender sender.o receiver receiver.o crc32.o cs431vde.o