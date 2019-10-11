sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

tst-cond:
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

archive:
	ar cr tst.a $(objects)
	cp tst.a ../

clean:
	rm *.o
	rm *.a
