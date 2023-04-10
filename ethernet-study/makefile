CFLAGS = -g -Wall -pedantic -Wno-pointer-arith 

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

binexpback: binexpback.o
	gcc $(CFLAGS) -lm -o binexpback binexpback.o

switch: switch.o
	gcc $(CFLAGS) -o switch switch.o

hub: hub.o
	gcc $(CFLAGS) -o hub hub.o