all: info tuFS.ext2
GLIBC_CRT_PATH=../../build-glibc/glibc-build/csu/
# Need to build on this machine.
LIBGCC_PATH=../../gcc-build-cpp/x86_64-pc-linux-gnu/libgcc/

CRT_STARTS= $(GLIBC_CRT_PATH)/crt1.o $(GLIBC_CRT_PATH)/crti.o #$(LIBGCC_PATH)/crtbegin.o

CRT_ENDS= $(LIBGCC_PATH)/crtend.o $(GLIBC_CRT_PATH)/crtn.o

GLIBC_PATH=../../build-glibc/glibc-build/

# TODO fix this name
GLIBC_AUX_LIBS= $(GLIBC_PATH)/nptl/libpthread.a $(GLIBC_PATH)/math/libm.a
GLIBC = $(GLIBC_PATH)/libc.a

GCC_LIBS= $(LIBGCC_PATH)/libgcc.a $(LIBGCC_PATH)/libgcc_eh.a

MY_LD_FLAGS= --allow-multiple-definition --defsym=__pthread_initialize_minimal=__pthread_initialize_minimal_internal

LD_DEBUG=--verbose -Map=out.map

MY_LD_FLAGS+= $(LD_DEBUG)

LOG_OUT= > linker_verbose.txt #2> (tee linker_err.txt >&2) #

# XXX Did we build this mcmodel=kernel?
# PERF_PATH=/root/perf/tools/perf/
# PERF_OBJS= $(PERF_PATH)/perf-in.o $(PERF_PATH)/pmu-events/pmu-events-in.o
# PERF_LIB_PATH=/root/perf/tools/lib
# PERF_LIBS= $(PERF_LIB_PATH)/api/libapi.a $(PERF_LIB_PATH)/traceevent/libtraceevent.a $(PERF_LIB_PATH)/subcmd/libsubcmd.a $(PERF_LIB_PATH)/perf/libperf.a

# This builds the partially linked app library that's ready to
# be finally linked with the modified linux kernel.
partial.o: printDirs.o $(CRT_STARTS) $(CRT_ENDS) $(GLIBC_LIBS)
# Really unclear if this is needed
	gcc -c -o fsb.o ../fsbringup.c -mcmodel=kernel -ggdb -mno-red-zone

# This will be used as UKL.a
	ld -r -o $@ $(MY_LD_FLAGS) $(CRT_STARTS) \
		$< fsb.o \
		--whole-archive $(GLIBC_AUX_LIBS) $(GLIBC) --no-whole-archive \
		--start-group $(GCC_LIBS) --end-group $(CRT_ENDS) $(LOG_OUT)

# --start-group $(GLIBC_AUX_LIBS)  --end-group \
# --start-group $(GCC_LIBS) $(GLIBC) --end-group $(CRT_ENDS) $(LOG_OUT)

# perf_partial.o: perf.o $(CRT_STARTS) $(CRT_ENDS) $(GLIBC_LIBS)
# 	gcc -c -o fsb.o ../fsbringup.c -mcmodel=kernel -ggdb -mno-red-zone


# # This will be used as UKL.a
# 	ld -r -o $@ $(MY_LD_FLAGS) $(CRT_STARTS) \
# 		$< fsb.o $(PERF_OBJS) \
# 		--whole-archive $(PERF_LIBS) $(GLIBC_LIBS) --no-whole-archive $(CRT_ENDS) $(LOG_OUT)

%.o: %.c
# Assemble the app Special flags for running in the kernel.
	gcc -Dprintf=printk -c -o $@ $< -mcmodel=kernel -ggdb -mno-red-zone

tuFS.ext2:
	fallocate -l 1M emptyFile
	mkfs.ext2 emptyFile
	mv emptyFile $@
	mkdir mt
	mount $@ mt
	touch mt/hi.txt
	umount mt

info: partial.o
	nm $< > all_syms.txt
	nm -u $< > unresolved_syms.txt
	objdump -d $< > objdump.txt
	file $< > file.txt
	md5sum $< > md5.txt
# Container for linux files after final link is later done.
	mkdir finished_linux_files
	mkdir info
	mv *.txt info
	mv *.map info

clean:
	rm -rf info finished_linux_files
	rm -rf *.o out *.ext2 mt
