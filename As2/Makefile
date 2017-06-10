GCC=gcc 
GCCFLAGS=  -pthread 

final: as2

as2: as2.o
	$(GCC) -o as2 $(GCCFLAGS) as2.o

clean: 
	rm -f as2 as2.o as2.out
