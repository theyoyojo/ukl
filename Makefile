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

# lebench: glibc
# 	gcc -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
# 	make -C ../linux M=$(PWD)
# 	ld -r -o lebenchfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition lebench.o --start-group glibcfinal --end-group 
# 	ar cr UKL.a ukl.o interface.o lebenchfinal.o
# 	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
# 	rm -rf ../linux/vmlinux 
# 	make -C ../linux -j$(shell nproc)

CRT_STARTS= $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o
CRT_ENDS= $(GLIBC_CRT_PATH)/crtn.o $(LIBGCC_PATH)/crtend.o

GLIBC_CRT_PATH=/root/MPC-UKL/C-Constructor/helpers/
GLIBC_LIBS= $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a $(MATH_PATH)/libm.a libc.a 
MY_LD_FLAGS= --unresolved-symbols=ignore-all --allow-multiple-definition --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal
lebench: glibc
	gcc -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)

# If you link libpthread as a .a as opposed to using the glibcfinal way
# you can remove the init minimal line. The unresolved symbols flag shouldn't
# do anything because it's a partial link.
	ld -r -o lebenchfinal.o \
	$(MY_LD_FLAGS) \
	$(CRT_STARTS) \
	lebench.o \
	--start-group glibcfinal --end-group \
	$(CRT_ENDS)

	ar cr UKL.a ukl.o interface.o lebenchfinal.o

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

PERF_PATH=/root/perf/tools/perf/
PERF_LIB_PATH=/root/perf/tools/lib
BENCH_PATH=/root/perf/tools/perf/work/
MATH_PATH=/root/unikernel/build-glibc/glibc-build/math/
LIBGCC_PATH=/root/MPC-UKL/C-Constructor/helpers/



PERF_OBJS= b-p.o $(PERF_PATH)/perf-in.o $(PERF_PATH)/pmu-events/pmu-events-in.o

PERF_LIBS= $(PERF_LIB_PATH)/api/libapi.a $(PERF_LIB_PATH)/traceevent/libtraceevent.a $(PERF_LIB_PATH)/subcmd/libsubcmd.a $(PERF_LIB_PATH)/perf/libperf.a 

UKL_OBJS= fsbringup.o ukl.o interface.o




benchmark-perf: glibc
# Build perf with mcmodel=kernel and no-red-zone
	rm -rf b-p.o b-pfinal.o
	gcc $(BENCH_PATH)/benchmark-perf.c -Dprintf=printk -c -o b-p.o -mcmodel=kernel -ggdb -mno-red-zone
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux M=$(PWD)

	ld -r -o b-pfinal.o $(MY_LD_FLAGS) $(CRT_STARTS) $(PERF_OBJS) $(UKL_OBJS) --start-group $(PERF_LIBS) $(GLIBC_LIBS) --end-group $(CRT_ENDS)

# --whole-archive $(PERF_LIB_PATH)/api/libapi.a $(PERF_LIB_PATH)/traceevent/libtraceevent.a $(PERF_LIB_PATH)/subcmd/libsubcmd.a $(PERF_LIB_PATH)/perf/libperf.a --no-whole-archive \
# ukl.a down to 115 from 297
# 227 with separate 297 with glibcfinal
	ar cr UKL.a b-pfinal.o
# ar cr UKL.a ukl.o interface.o b-pfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux -j$(shell nproc)
# libc.a libpthread.a --end-group \
# glibcfinal --end-group \
#				\
# glibcfinal
#$(LIBGCC_PATH)/crtend.o $(LIBGCC_PATH)/crtbegin.o 
	# ld -r -o glibcfinal --unresolved-symbols=ignore-all --allow-multiple-definition --whole-archive fsbringup.o libc.a libpthread.a --no-whole-archive

CCON_PATH=/root/MPC-UKL/C-Constructor/
cpp: glibc
	rm -rf cons.o cpptestfinal.o
	gcc $(CCON_PATH)/cons.c -c -o cons.o -mcmodel=kernel -ggdb -mno-red-zone
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux M=$(PWD)
	ld -r -o cpptestfinal.o --unresolved-symbols=ignore-all --allow-multiple-definition \
		--defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
        $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o \
        cons.o \
        --start-group $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a glibcfinal --end-group \
        $(LIBGCC_PATH)/crtend.o $(GLIBC_CRT_PATH)/crtn.o 
# 	ld -r -o cpptestfinal.o --defsym=__init_array_start=.init_array cpptestfinal1.o
	ar cr UKL.a ukl.o interface.o cpptestfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux -j$(shell nproc)


# lebench: glibc
# 	gcc -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
# 	make -C ../linux M=$(PWD)

# 	ld -r -o lebenchfinal.o \
# 	$(MY_LD_FLAGS) $(CRT_STARTS) lebench.o $(UKL_OBJS) \
# 	--start-group $(GLIBC_LIBS) --end-group $(CRT_ENDS)

# 	ar cr UKL.a lebenchfinal.o
# 	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a
# 	rm -rf ../linux/vmlinux
# 	make -C ../linux -j$(shell nproc)

singlethreaded-tcp-server: glibc
	gcc tcpsingle.c -c -o tcpsingle.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
	ld -r -o mytcp.o --unresolved-symbols=ignore-all --allow-multiple-definition tcpsingle.o --start-group glibcfinal --end-group 
	ar cr UKL.a ukl.o interface.o mytcp.o 
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

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
