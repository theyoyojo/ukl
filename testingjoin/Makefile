sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

archive: tst-cond
	ar cr tst.a $(objects)
	cp tst.a ../

tst-cond: clean
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

clean:
	rm *.o
	rm *.a
