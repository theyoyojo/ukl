obj-y += ukl.o interface.o


.PHONY: glibc cj utb4 barriercheck

glibc:
	./extractglibc.sh
	gcc -c -o fsbringup.o fsbringup.c -mcmodel=kernel -ggdb -mno-red-zone
	rm -rf UKL.a
	ld -r -o glibcfinal --unresolved-symbols=ignore-all --allow-multiple-definition --whole-archive fsbringup.o libc.a libpthread.a --no-whole-archive

fstest: glibc
	gcc -c -o fsbringup.o fsbringup.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o fsfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition fsbringup.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o fsfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

memmoveCheck: glibc
	gcc -c -o memmovecheck.o memmovecheck.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o mmcfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition memmovecheck.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o mmcfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

gettimecheck: glibc
	gcc -c -o gettimecheck.o gettimecheck.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o gtfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition gettimecheck.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o gtfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

lebench: glibc
	gcc -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)
	ld -r -o lebenchfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition lebench.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o lebenchfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

test: glibc
	rm -rf tst*
	cp /home/fedora/unikernel/build-glibc/glibc/nptl/${case}.c .
	gcc -c -o ${case}.o ${case}.c -mcmodel=kernel -ggdb -Wno-implicit
	gcc -c -o testingmain.o testingmain.c -mcmodel=kernel -ggdb -Wno-implicit
	ld -r -o test.o testingmain.o ${case}.o
	make -C ../linux M=$(PWD)
	ld -r -o testfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition interface.o test.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o testfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

stackextend: glibc
	gcc -c -o stackextend.o stackextend.c -mcmodel=kernel -ggdb -Wno-implicit
	make -C ../linux M=$(PWD)
	ld -r -o sefinal.o --unresolved-symbols=ignore-all --allow-multiple-definition interface.o stackextend.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o sefinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

cj:
	gcc -o cj cj.c -lpthread -ggdb

canceljoin: glibc
	gcc -c -o canceljoin.o canceljoin.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o cjfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition canceljoin.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o cjfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

stack: glibc
	gcc -c -o rspcheck.o rspcheck.S -mcmodel=kernel -ggdb
	gcc -c -o stackcheck.o stackcheck.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o stackfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition stackcheck.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o stackfinal.o rspcheck.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

uklfutex: glibc
	gcc -c -o rspcheck.o rspcheck.S -mcmodel=kernel -ggdb
	gcc -c -o stackcheck.o stackcheck.c -mcmodel=kernel -ggdb
	gcc -c -o uklfutex.o uklfutex.c -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o uklfutexfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition uklfutex.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o uklfutexfinal.o rspcheck.o stackcheck.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

memcached: glibc
	rm -f UKLmemcached
	rm -f UKLlibevent
	cp ../memcached/UKLmemcached .
	cp ../libevent/UKLlibevent .
	make -C ../linux M=$(PWD)
	ld -r -o memcachedfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition UKLmemcached --start-group glibcfinal UKLlibevent --end-group 
	ar cr UKL.a ukl.o interface.o memcachedfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

multithreaded-tcp-server: glibc
	gcc multithreadedserver.c -c -o multithreadedserver.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o multcp.o --unresolved-symbols=ignore-all --allow-multiple-definition multithreadedserver.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o multcp.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

multithreaded-printing: glibc
	gcc printer.c -c -o printer.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o mulprint.o --unresolved-symbols=ignore-all --allow-multiple-definition printer.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o mulprint.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

client:
	gcc -o client client.c -lpthread -ggdb --static

userstack:
	gcc -o userstack userstack.c rspcheck.S -lpthread -ggdb --static

ufutex:
	gcc -o ufutex ufutex.c -lpthread -ggdb --static


singlethreaded-tcp-server: glibc
	gcc tcpsingle.c -c -o tcpsingle.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o mytcp.o --unresolved-symbols=ignore-all --allow-multiple-definition tcpsingle.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o mytcp.o 
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

fs_test: glibc
	gcc fs_test.c -c -o fs_test.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o fs_testp.o --unresolved-symbols=ignore-all --allow-multiple-definition fs_test.o --start-group glibcfinal --end-group
	ar cr UKL.a ukl.o interface.o fs_test.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a
	rm -rf ../linux/vmlinux
	make -C ../linux -j$(shell nproc)

ppid_test: glibc
	time gcc -Wall ppid_test.c -c -o ppid_test.o -mcmodel=kernel -ggdb
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux M=$(PWD)
# time make -C ../linux M=$(PWD)
	time ld -r -o ppid_testp.o --unresolved-symbols=ignore-all --allow-multiple-definition ppid_test.o --start-group glibcfinal --end-group
	time ar cr UKL.a ukl.o interface.o ppid_testp.o
	time rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a
	time rm -rf ../linux/vmlinux
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux -j $(nproc)
# time make -C ../linux -j$(shell nproc)
# mv bzImage here as tuPPID


run:
	make -C ../min-initrd runU

debug:
	make -C ../min-initrd debugU

mon:
	make -C ../min-initrd monU

utb4:
	rm utb4
	gcc -o utb4 utb4.c -lpthread -ggdb

barriercheck:
	rm -rf barriercheck
	gcc -o barriercheck barriercheck.c -lpthread -ggdb
