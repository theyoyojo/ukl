sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

tst-cond :
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

archive:
	ar cr tstcond.a $(objects)

clean:
	rm *.o
