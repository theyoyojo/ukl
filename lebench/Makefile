all: lebench_info
# GLIBC_CRT_PATH=/root/MPC-UKL/C-Constructor/helpers/
GLIBC_CRT_PATH=/root//unikernel/build-glibc/glibc-build/csu

# Need to build on this machine.
LIBGCC_PATH=/root/MPC-UKL/C-Constructor/helpers

CRT_STARTS= $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o $(LIBGCC_PATH)/crtbegin.o

CRT_ENDS= $(GLIBC_CRT_PATH)/crtn.o $(LIBGCC_PATH)/crtend.o

GLIBC_PATH=/root/unikernel/build-glibc/glibc-build

GLIBC_LIBS= $(GLIBC_PATH)/libc.a $(GLIBC_PATH)/nptl/libpthread.a
#$(LIBGCC_PATH)/libgcc.a #$(LIBGCC_PATH)/libgcc_eh.a

MY_LD_FLAGS= --unresolved-symbols=ignore-all --allow-multiple-definition --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal

LD_DEBUG=--verbose -Map=lebench.map

MY_LD_FLAGS+= $(LD_DEBUG)

LOG_OUT= > linker_verbose.txt #2> (tee linker_err.txt >&2) #

UKL_MISC= ../fsbringup.o

# LINUX_LIBS=arch/x86/kernel/head_64.o arch/x86/kernel/head64.o arch/x86/kernel/ebda.o arch/x86/kernel/platform-quirks.o init/built-in.a usr/built-in.a arch/x86/built-in.a kernel/built-in.a certs/built-in.a mm/built-in.a fs/built-in.a ipc/built-in.a security/built-in.a crypto/built-in.a block/built-in.a lib/built-in.a arch/x86/lib/built-in.a lib/lib.a arch/x86/lib/lib.a drivers/built-in.a sound/built-in.a arch/x86/pci/built-in.a arch/x86/power/built-in.a arch/x86/video/built-in.a net/built-in.a virt/built-in.a

# LINUX_SYMS=../../linux/.tmp_vmlinux.kallsyms2.o

# LL_FULL_PATH=$(addprefix ../../linux/, $(LINUX_LIBS))

# resolve_with_linux.o: lebench_partial.o
# 	 ld -r -o $@ $(MY_LD_FLAGS) $< \
# 		--start-group  $(LL_FULL_PATH) --end-group $(LINUX_SYMS)

# This builds the partially linked app library that's ready to
# be finally linked with the modified linux kernel.
lebench_partial.o: lebench.o $(CRT_STARTS) $(CRT_ENDS) $(GLIBC_LIBS)
# Build UKL lib
	ld -r -o $@ $(MY_LD_FLAGS) $(CRT_STARTS) $< $(UKL_MISC) \
		--whole-archive $(GLIBC_LIBS) --no-whole-archive \
		$(CRT_ENDS) $(LOG_OUT)

lebench.o: lebench.c
# Assemble the app Special flags for running in the kernel.
	gcc -c -o lebench.o $< -mcmodel=kernel -ggdb -mno-red-zone

lebench_info: lebench_partial.o
	nm $< > lebench_all_syms.txt
	nm -u $< > lebench_unresolved_syms.txt
	objdump -d $< > lebench_objdump.txt
	file $< > lebench_file.txt
	md5sum $< > lebench_md5.txt
# Container for linux files after final link is later done.
	mkdir finished_linux_files
	mkdir info
	mv *.txt info
	mv *.map info

clean:
	rm -rf info finished_linux_files
	rm -rf lebench.o lebench_partial.o
