.PHONY: glibc_copy

# General linking order:
# crt1.o crti.o crtbegin.o [-L paths] [user objects] {whole archive: [C libs] [C++ libs] } {start/end group: [gcc libs] } crtend.o crtn.o

GLIBC_CRT_PATH = ../../build-glibc/glibc-build/csu
LIBGCC_PATH = ../../gcc-install/lib/gcc/x86_64-pc-linux-gnu/8.3.0
CPP_PATH = ../../gcc-install/lib64

CRT_STARTS = $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o
CRT_ENDS = $(LIBGCC_PATH)/crtend.o $(GLIBC_CRT_PATH)/crtn.o

GLIBC_LIBS = libc.a libpthread.a
LIBGCC_LIBS = $(LIBGCC_PATH)/libgcc.a  #$(LIBGCC_PATH)/libgcc_eh.a #_eh not built by default...gcc must be built a 2nd time with "--disabled-shared" removed to build it
CPP_LIB = $(CPP_PATH)/libstdc++.a  # Standard C++ library that should always be included as "whole archive"
CPP_OPT_FS = $(CPP_PATH)/libstdc++fs.a  # Optional filesystem library that is sometimes needed (can go in start/end group)
CPP_OPT_SUP = $(CPP_PATH)/libsupc++.a  # Optional support/unwind library that is sometimes needed (should go in whole archive if needed)

MY_LD_FLAGS = --unresolved-symbols=ignore-all --allow-multiple-definition --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal

glibc_copy:
	rm -rf libc.a libpthread.a
	cp ../../build-glibc/glibc-build/libc.a .
	cp ../../build-glibc/glibc-build/nptl/libpthread.a .

c_constructors: glibc_copy
	rm -rf cons.o UKL.a
	gcc cons.c -c -o cons.o -mcmodel=kernel -ggdb -mno-red-zone
	ld -r -o UKL.a $(MY_LD_FLAGS) $(CRT_STARTS) \
		cons.o \
		--whole-archive $(GLIBC_LIBS) --no-whole-archive \
		$(CRT_ENDS)
	cp UKL.a ../
	rm -rf ../../linux/vmlinux && make -C ../../linux -j$(shell nproc)

c_fs_startup: glibc_copy
	rm -rf cstart.o fsroot.o UKL.a
	gcc -c -o cstart.o startup.c -mcmodel=kernel -ggdb -mno-red-zone -Dprintf=printk
	gcc -c -o fsroot.o fsroot.c -mcmodel=kernel -ggdb -mno-red-zone
	ld -r -o UKL.a $(MY_LD_FLAGS) $(CRT_STARTS) cstart.o fsroot.o \
		--whole-archive $(GLIBC_LIBS) --no-whole-archive \
		$(CRT_ENDS)
	cp UKL.a ../
	rm -rf ../../linux/vmlinux && make -C ../../linux -j$(shell nproc)

hellocpp: glibc_copy
	rm -rf hello_final.o UKL.a
	gcc hello.cpp -c -o hello_final.o -mcmodel=kernel -ggdb -mno-red-zone
	ld -r -o UKL.a $(MY_LD_FLAGS) $(CRT_STARTS) \
		hello_final.o \
		--whole-archive $(GLIBC_LIBS) $(CPP_LIB) --no-whole-archive \
		$(CRT_ENDS) \
		--start-group $(LIBGCC_LIBS) --end-group
	cp UKL.a ../
	rm -rf ../../linux/vmlinux && make -C ../../linux -j$(shell nproc)

cpp_filesystem: glibc_copy
	rm -rf cppfs_final.o fsroot.o UKL.a
	gcc cppfs.cpp -c -o cppfs_final.o -mcmodel=kernel -ggdb -mno-red-zone
	gcc -c -o fsroot.o fsroot.c -mcmodel=kernel -ggdb -mno-red-zone
	ld -r -o UKL.a $(MY_LD_FLAGS) $(CRT_STARTS) \
		cppfs_final.o fsroot.o \
		--whole-archive $(GLIBC_LIBS) $(CPP_LIB) --no-whole-archive \
		$(CRT_ENDS) \
		--start-group $(LIBGCC_LIBS) --end-group
	cp UKL.a ../
	rm -rf ../../linux/vmlinux && make -C ../../linux -j$(shell nproc)

run:
	make -C ../../min-initrd runU
