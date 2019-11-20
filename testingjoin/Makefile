sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

archive: tst
	ar cr tst.a $(objects)
	cp tst.a ../
	nm tst.a

tst: clean
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

clean:
	rm -rf *.o
	rm -rf *.a
