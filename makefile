CFLAGS=-lssl -lcrypto
anticheat-dev: main.o anticheat.o hashUtil.o
	gcc -g -o anticheat-dev main.o anticheat.o hashUtil.o $(CFLAGS)

anticheat.o: anticheat.c
	gcc -g -c -o anticheat.o anticheat.c

main.o: main.c
	gcc -g -c -o main.o main.c

hashUtil.o: hashUtil.c
	gcc -g -c -o hashUtil.o hashUtil.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f anticheat-dev


