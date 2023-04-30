CFLAGS=-lssl -lcrypto
anticheat-dev: main.o anticheat.o hashUtil.o
	gcc -o anticheat main.o anticheat.o hashUtil.o $(CFLAGS)

anticheat.o: anticheat.c
	gcc -c -o anticheat.o anticheat.c

main.o: main.c
	gcc -c -o main.o main.c

hashUtil.o: hashUtil.c
	gcc -c -o hashUtil.o hashUtil.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f anticheat


