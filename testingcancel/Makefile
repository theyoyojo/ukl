sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

archive: tst
	ar cr tst.a *.o
	cp tst.a ../

tst: clean
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

clean:
	rm -f *.o
	rm -f *.a
