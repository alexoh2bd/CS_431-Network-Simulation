CFLAGS = -g -Wall -pedantic -Wno-pointer-arith

# VPATH = src/ethernet-study
# DIR = cs431/ethernet-study




all: hexdump hexread sender receiver
.Phony: all

stacksend: stack sender
.Phony: $^

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

#(DIR)/%.o: %.c
#	gcc $(CFLAGS) -c -o $@ $^



stack: stack.o util.o cs431vde.o
	gcc $(CFLAGS) -o stack stack.o util.o cs431vde.o

hexdump: hexdump.o util.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o

hexread: hexread.o util.o
	gcc $(CFLAGS) -o hexread hexread.o util.o

sender: sender.o util.o cs431vde.o
	gcc $(CFLAGS) -o sender sender.o util.o cs431vde.o

receiver: receiver.o util.o cs431vde.o
	gcc $(CFLAGS) -o receiver receiver.o util.o cs431vde.o


.Phony: clean
clean:
	rm -f util hexdump util.o hexdump.o hexread hexread.o stack stack.o sender sender.o receiver receiver.o