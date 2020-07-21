# These are built in the make -C ../linux M=$(PWD) step.
# Is it using linux specific config???

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


# ukl.o:
# 	cd ../linux; gcc -Wp,-MD,/root/unikernel/ukl/.ukl.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-redhat-linux/8/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Wno-format-security -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_SSSE3=1 -DCONFIG_AS_AVX=1 -DCONFIG_AS_AVX2=1 -DCONFIG_AS_AVX512=1 -DCONFIG_AS_SHA1_NI=1 -DCONFIG_AS_SHA256_NI=1 -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -Wimplicit-fallthrough -Wno-unused-const-variable -fomit-frame-pointer -fno-var-tracking-assignments -g -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -fmacro-prefix-map=./= -Wno-packed-not-aligned    -DKBUILD_BASENAME='"ukl"' -DKBUILD_MODNAME='"ukl"' -c -o /root/unikernel/ukl/ukl.o /root/unikernel/ukl/ukl.c #; scripts/basic/fixdep /root/unikernel/ukl/.ukl.o.d /root/unikernel/ukl/ukl.o

# interface.o:
# 	cd ../linux; gcc -Wp,-MD,/root/unikernel/ukl/.interface.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-redhat-linux/8/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Wno-format-security -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_SSSE3=1 -DCONFIG_AS_AVX=1 -DCONFIG_AS_AVX2=1 -DCONFIG_AS_AVX512=1 -DCONFIG_AS_SHA1_NI=1 -DCONFIG_AS_SHA256_NI=1 -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -Wimplicit-fallthrough -Wno-unused-const-variable -fomit-frame-pointer -fno-var-tracking-assignments -g -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -fmacro-prefix-map=./= -Wno-packed-not-aligned    -DKBUILD_BASENAME='"interface"' -DKBUILD_MODNAME='"interface"' -c -o /root/unikernel/ukl/interface.o /root/unikernel/ukl/interface.c #; scripts/basic/fixdep /root/unikernel/ukl/.interface.o.d /root/unikernel/ukl/interface.o

LIBGCC_PATH=/root/MPC-UKL/C-Constructor/helpers/
GLIBC_CRT_PATH=~/unikernel/build-glibc/glibc-build/csu/
GLIBC_PATH=~/unikernel/build-glibc/glibc-build/

# lebench-tu: glibc #ukl.o interface.o
# 	gcc -Dkmain=main -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
# 	make -C ../linux M=$(PWD)

	# ld -r -o lebenchfinal.o \
	# --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	# --unresolved-symbols=ignore-all \
	# --allow-multiple-definition \
	# $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o \
	# lebench.o fsbringup.o $(GLIBC_PATH)/nptl/libpthread.a \
	# --start-group $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a $(GLIBC_PATH)/libc.a --end-group \
	# $(GLIBC_CRT_PATH)/crtn.o 

# $(LIBGCC_PATH)/crtbegin.o 
# $(LIBGCC_PATH)/crtend.o 

	# ld -r -o lebenchfinal.o \
	# --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	# --unresolved-symbols=ignore-all \
	# --allow-multiple-definition \
	# $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o \
	# lebench.o fsbringup.o $(GLIBC_PATH)/nptl/libpthread.a \
	# --start-group $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a $(GLIBC_PATH)/libc.a --end-group \
	# $(LIBGCC_PATH)/crtend.o $(GLIBC_CRT_PATH)/crtn.o


# Old working version
lebench-tu: glibc
	gcc -Dkmain=main -c -o lebench.o OS_Eval.c -mcmodel=kernel -ggdb -mno-red-zone
	make -C ../linux M=$(PWD)

	ld -r -o lebenchfinal.o \
	--defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	/usr/lib64/crt1.o /usr/lib64/crti.o \
	lebench.o \
	--start-group glibcfinal --end-group \
	/usr/lib64/crtn.o 

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

multithreaded-tcp-server-tu: glibc
	gcc -Dkmain=main multithreadedserver.c -c -o multithreadedserver.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)

	ld -r -o multcp.o \
	--defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	--defsym=TU_PATH=1 \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	/usr/lib64/crt1.o /usr/lib64/crti.o \
	multithreadedserver.o \
	/usr/lib64/crtn.o \
	--start-group glibcfinal --end-group

	ar cr UKL.a ukl.o interface.o multcp.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a
	rm -rf ../linux/vmlinux
	make -C ../linux -j$(shell nproc)

multithreaded-printing: glibc

	gcc printer.c -c -o printer.o -mcmodel=kernel -ggdb

	make -C ../linux M=$(PWD)

	ld -r -o mulprint.o \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	printer.o \
	--start-group glibcfinal --end-group # Keep as libs to minimize size.

	ar cr UKL.a ukl.o interface.o mulprint.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux
	make -C ../linux -j$(shell nproc)

multithreaded-printing-tu: glibc
	gcc printer.c -Dkmain=main -c -o printer.o -mcmodel=kernel -ggdb

	make -C ../linux M=$(PWD)

# HACK XXX Incomplete set of crt files!
	ld -r -o mulprint.o \
	--defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	--defsym=TU_PATH=1 \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	/usr/lib64/crt1.o /usr/lib64/crti.o \
	printer.o \
	/usr/lib64/crtn.o \
	--start-group glibcfinal --end-group # Keep as libs to minimize size.
# This results in  13M
# --start-group libc.a libpthread.a --end-group # Keep as libs to minimize size.
# this works, but is 46 M

# ld -r -o mulprint.o --unresolved-symbols=ignore-all --allow-multiple-definition printer.o  --start-group glibcfinal --end-group 

# Don't need interface.o if going through _start
	ar cr UKL.a ukl.o interface.o mulprint.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)
	# time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux -j $(nproc)
# C runtime setup files
# https://dev.gentoo.org/~vapier/crt.txt


# tu:
# 	echo $(CRT_PREFIX) myobj.o $(CRT_SUFFIX)
# --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal 


# Libs
GLIBC_CRT_PATH=~/unikernel/build-glibc/glibc-build/csu/
GLIBC_PATH=~/unikernel/build-glibc/glibc-build/
LIBGCC_PATH=/root/MPC-UKL/C-Constructor/helpers/

CRT_PREFIX := $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o 
CRT_SUFFIX := $(LIBGCC_PATH)/crtend.o $(GLIBC_CRT_PATH)/crtn.o

GCC_LIBS := $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a

tu_c_constructor: /root/MPC-UKL/C-Constructor/cons.c ukl.c interface.c
	gcc -c -o cons.o $< -mcmodel=kernel -ggdb -mno-red-zone

	make -C ../linux M=$(PWD)

	ld -r -o consfinal.o \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	$(CRT_PREFIX) \
	cons.o $(GLIBC_PATH)/nptl/libpthread.a \
	--start-group $(GCC_LIBS) $(GLIBC_PATH)/libc.a --end-group \
	$(CRT_SUFFIX)

	ar cr UKL.a ukl.o interface.o consfinal.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	make -C ../linux -j$(shell nproc)

# --unresolved-symbols=ignore-all --allow-multiple-definition /usr/lib64/crt1.o  /usr/lib64/crti.o c_constructor.o  /usr/lib64/crtn.o --start-group glibcfinal --end-group #--defsym=__init_array_start=0xffffffff82559938 --defsym=__init_array_end=0xffffffff82559950



c_constructor: glibc
	gcc /root/MPC-UKL/C-Constructor/cons.c -c -o c_constructor.o -mcmodel=kernel -ggdb
	make -C ../linux M=$(PWD)
# no good frame_dummy relocations fucked up __TMC_END__ isn't defined.
# ld -r -o c_constructor_plus_libs.o --unresolved-symbols=ignore-all --allow-multiple-definition $(CRT_PREFIX) c_constructor.o $(CRT_SUFFIX) --start-group glibcfinal --end-group #--defsym=__init_array_start=0xffffffff82559938 --defsym=__init_array_end=0xffffffff82559950
# Gets through main, think doesn't run constructor
	# gcc -r -mcmodel=kernel -ggdb -o c_constructor_plus_libs.o c_constructor.o glibcfinal #-Wl,--unresolved-symbols=ignore-all -Wl,--allow-multiple-definition  #--defsym=__init_array_start=0xffffffff82559938 --defsym=__init_array_end=0xffffffff82559950
# Gets through main, think doesn't run constructor
	ld -r -o c_constructor_plus_libs.o --unresolved-symbols=ignore-all --allow-multiple-definition /usr/lib64/crt1.o  /usr/lib64/crti.o c_constructor.o  /usr/lib64/crtn.o --start-group glibcfinal --end-group #--defsym=__init_array_start=0xffffffff82559938 --defsym=__init_array_end=0xffffffff82559950
	# gcc -mcmodel=kernel -r -o c_constructor_plus_libs.o -Wl,--unresolved-symbols=ignore-all -Wl,--allow-multiple-definition c_constructor.o /usr/lib64/crt1.o  /usr/lib64/crti.o /usr/lib64/crtn.o --start-group glibcfinal --end-group
# HACK XXX order of crt files! I don't know!
	# time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux M=$(PWD)

# This built but didn't boot as expected.
	# gcc -static -o c_constructor_plus_libs.o -Wl,--unresolved-symbols=ignore-all -Wl,--allow-multiple-definition /root/MPC-UKL/C-Constructor/cons.c /usr/lib64/crt1.o /usr/lib64/crti.o /usr/lib64/crtn.o -Wl,--start-group glibcfinal -Wl,--end-group -mcmodel=kernel

	ar cr UKL.a ukl.o interface.o c_constructor_plus_libs.o
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 
	rm -rf ../linux/vmlinux 
	time KBUILD_BUILD_TIMESTAMP='' CC="ccache gcc" make -C ../linux -j $(nproc)
#  [32] .init_array       INIT_ARRAY      ffffffff82559938 1759938 000018 08  WA  0   0  8
client:
	gcc -o client client.c -lpthread -ggdb --static

userstack:
	gcc -o userstack userstack.c rspcheck.S -lpthread -ggdb --static

ufutex:
	gcc -o ufutex ufutex.c -lpthread -ggdb --static


singlethreaded-tcp-server: glibc
# Building a .o
# gcc -Dkmain=main tcpsingle.c -c -o tcpsingle.o -mcmodel=kernel -ggdb
	gcc tcpsingle.c -c -o tcpsingle.o -mcmodel=kernel -ggdb

# Ask linux makesystem to build interface.o and ukl.o
	make -C ../linux M=$(PWD)

# App with glibc fsbringup and libpthread
	ld -r -o mytcp.o \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	tcpsingle.o \
	--start-group glibcfinal --end-group # Keep as libs to minimize size.

# --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
# /usr/lib64/crt1.o /usr/lib64/crti.o \
# /usr/lib64/crtn.o \
# build UKL.a archive
	ar cr UKL.a ukl.o interface.o mytcp.o

# Cleanup from step 2
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 

# Remove exec
	rm -rf ../linux/vmlinux 

# Rebuild linux
	make -C ../linux -j$(shell nproc)

singlethreaded-tcp-server-tu: glibc
# Building a .o
	gcc -Dkmain=main tcpsingle.c -c -o tcpsingle.o -mcmodel=kernel -ggdb

# Ask linux makesystem to build interface.o and ukl.o
	make -C ../linux M=$(PWD)

# App with glibc fsbringup and libpthread
	ld -r -o mytcp.o \
	--defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal \
	--defsym=TU_PATH=1 \
	--unresolved-symbols=ignore-all \
	--allow-multiple-definition \
	/usr/lib64/crt1.o /usr/lib64/crti.o \
	tcpsingle.o \
	/usr/lib64/crtn.o \
	--start-group glibcfinal --end-group # Keep as libs to minimize size.

# build UKL.a archive
	ar cr UKL.a ukl.o interface.o mytcp.o

# Cleanup from step 2
	rm -rf *.ko *.mod.* .H* .tm* .*cmd Module.symvers modules.order built-in.a 

# Remove exec
	rm -rf ../linux/vmlinux 

# Rebuild linux
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
