obj-y += interface.o


.PHONY: glibc cj utb4 barriercheck

glibc:
	./extractglibc.sh
	rm -rf UKL.a
	ld -r -o glibcfinal --unresolved-symbols=ignore-all --allow-multiple-definition --whole-archive crti.o libc.a libpthread.a start.o crtn.o --no-whole-archive

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

LEB_DIR =./lebench/
lebench_app:
# Remove this once you figure it out.
	gcc -c -o fsbringup.o fsbringup.c -mcmodel=kernel -ggdb
# Remove all old state
	make -C lebench clean

# Build lebench app lib
	make -C lebench

# Linux link script expects to find UKL.a here.
	cp $(LEB_DIR)/UKL.a ./UKL.a

# Force rebuild of vmlinux, pulls in UKL.a in link script
	rm -rf ../linux/vmlinux && make -C ../linux -j$(shell nproc)

# Copy vmlinux and compressed linux into dir for later inspection.
	# cp ../linux/vmlinux $(LEB_DIR)/finished_linux_files
	cp ../linux/arch/x86/boot/bzImage $(LEB_DIR)/finished_linux_files
	cp ../linux/System.map $(LEB_DIR)/finished_linux_files

PERF_DIR =perf
perf_app: perf
# Remove all old state
	make -C $< clean

# Build lebench app lib
	make -C $<

# Linux link script expects to find UKL.a here.
# cp $(PERF_DIR)/perf_partial.o ./UKL.a
	cp $(PERF_DIR)/UKL.a .

# Force rebuild of vmlinux, pulls in UKL.a in link script
	rm -rf ../linux/vmlinux && make -C ../linux -j$(shell nproc)

# Copy vmlinux and compressed linux into dir for later inspection.
	# cp ../linux/vmlinux $(LEB_DIR)/finished_linux_files
	cp ../linux/arch/x86/boot/bzImage $(PERF_DIR)/finished_linux_files
	cp ../linux/System.map $(PERF_DIR)/finished_linux_files



PRINT_DIR =printDirs
printDir_app: printDirs
# Remove all old state
	make -C $< clean

# Build lebench app lib
	make -C $<

# Linux link script expects to find UKL.a here.
	cp $(PRINT_DIR)/partial.o ./UKL.a

# Force rebuild of vmlinux, pulls in UKL.a in link script
	rm -rf ../linux/vmlinux && make -C ../linux -j$(shell nproc)

# Copy vmlinux and compressed linux into dir for later inspection.
	# cp ../linux/vmlinux $(LEB_DIR)/finished_linux_files
	cp ../linux/arch/x86/boot/bzImage $(PRINT_DIR)/finished_linux_files
	cp ../linux/System.map $(PRINT_DIR)/finished_linux_files

ASPRINTF_DIR =asprintf
asprintf_app: $(ASPRINTF_DIR)
# Remove all old state
	make -C $< clean

# Build lebench app lib
	make -C $<

# Linux link script expects to find UKL.a here.
	cp $(ASPRINTF_DIR)/partial.o ./UKL.a

# Force rebuild of vmlinux, pulls in UKL.a in link script
	rm -rf ../linux/vmlinux && make -C ../linux -j$(shell nproc)

# Copy vmlinux and compressed linux into dir for later inspection.
	# cp ../linux/vmlinux $(LEB_DIR)/finished_linux_files
	cp ../linux/arch/x86/boot/bzImage $(ASPRINTF_DIR)/finished_linux_files
	cp ../linux/System.map $(ASPRINTF_DIR)/finished_linux_files

asprintf_run:
	# SMOptions='foo' make -C ../min-initrd runU
	SMOptions='-initrd min-initrd.d/initrd -hda ../ukl/asprintf/tuFS.ext2' make -C ../min-initrd runU

malloctest: glibc
	gcc -c -o malloctest.o malloctest.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)
	ld -r -o malloctestfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition malloctest.o --start-group glibcfinal --end-group 
	ar cr UKL.a interface.o malloctestfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

writetest: glibc
	gcc -c -o wt.o write_test.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)
	ld -r -o wtfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition wt.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o wtfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

signaltest: glibc
	gcc -c -o signaltest.o sigtest.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)
	ld -r -o signalfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition signaltest.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o signalfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

test: glibc
	rm -rf tst*
	cp ../build-glibc/glibc/nptl/${case}.c .
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
	#make -C ../linux -j$(shell nproc)

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
perf_run:
	SMOptions='-initrd min-initrd.d/initrd -hda ../ukl/printDirs/tuFS.ext2' make -C ../min-initrd runU
printDir_run:
	# SMOptions='foo' make -C ../min-initrd runU
	SMOptions='-initrd min-initrd.d/initrd -hda ../ukl/printDirs/tuFS.ext2' make -C ../min-initrd runU

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
