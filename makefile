CFLAGS = -g -Wall -pedantic

all: hexdump 
.Phony: all

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

hexdump: hexdump.o util.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o


#util: util.o 
#	gcc $(CFLAGS) -o util util.o



.Phony: clean
clean:
	rm -f util hexdump