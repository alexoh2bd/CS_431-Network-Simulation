CFLAGS = -g -Wall -pedantic -Wno-pointer-arith

all: hexdump hexread
.Phony: all

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

hexdump: hexdump.o util.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o

hexread: hexread.o util.o
	gcc $(CFLAGS) -o hexread hexread.o util.o
	
#util: util.o 
#	gcc $(CFLAGS) -o util util.o



.Phony: clean
clean:
	rm -f util hexdump util.o hexdump.o hexread hexread.o