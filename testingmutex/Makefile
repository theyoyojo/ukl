sources := $(wildcard *.c)
objects := $(wildcard *.o)

.PHONY: clean

archive: tst
	ar cr tst.a *.o
	rm -rf ../tst.a
	cp tst.a ../
	nm tst.a

tst: clean
	gcc -c $(sources) -mcmodel=kernel -ggdb -Wno-implicit

clean:
	rm -f *.o
	rm -f *.a
