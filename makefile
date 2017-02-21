all: y86emul

y86emul: y86emul.c memory.h
		gcc -Wall -o y86emul y86emul.c memory.h

clean:
	rm -f *.o
	rm -f y86emul
