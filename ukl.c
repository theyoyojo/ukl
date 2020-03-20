/*
 * ukl.c
 *
 * Copyright (C) 2018, Ali Raza <aliraza@bu.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <linux/ukl.h>

void ukl_save_regs(void){
	__asm__("pushq	%r15\n"
		"pushq	%r14\n"
		"pushq	%r13\n"
		"pushq	%r12\n"
		"pushq	%r11\n"
		"pushq	%r10\n"
		"pushq	%r8\n"
		"pushq	%r9\n"
		"pushq	%rdi\n"
		"pushq	%rsi\n"
		"pushq	%rbp\n"
		"pushq	%rdx\n"
		"pushq	%rcx\n"
		"pushq	%rbx\n"
		"pushq	%rax"
               );
}

void ukl_restore_regs(void){
	__asm__("popq  %rax\n"
                "popq  %rbx\n"
                "popq  %rcx\n"
                "popq  %rdx\n"
                "popq  %rbp\n"
                "popq  %rsi\n"
                "popq  %rdi\n"
                "popq  %r9\n"
                "popq  %r8\n"
                "popq  %r10\n"
                "popq  %r11\n"
                "popq  %r12\n"
                "popq  %r13\n"
                "popq  %r14\n"
		"popq  %r15"
	       );
}

void ukl_handle_signals(void){
	struct ksignal ksig;
	void (*ukl_handler)(int,...);

	ukl_save_regs();
	while (get_signal(&ksig)) {
		ukl_handler = (void*) ksig.ka.sa.sa_handler;
		ukl_handler(ksig.sig, &ksig.info, &ksig.ka.sa.sa_restorer);
	}
	ukl_restore_regs();
}

#ifdef CONFIG_PREEMPT_NONE
void enter_ukl(void)
{
       /*
	__asm__("cmp $0x0, %rsp\n"
	       "jl 1f\n"
	       "movq %rsp, %rax\n"
               "subq $100, %rax\n"
	       "movq %rax, 0x0(%rax)\n"
	       "subq $4096, %rax\n"
	       "movq %rax, 0x0(%rax)\n"
	       "subq $8192, %rax\n"
	       "movq %rax, 0x0(%rax)\n"
	       "subq $12288, %rax\n"
	       "movq %rax, 0x0(%rax)\n"
	       "1:"
	       );
	*/
       exit_application();
       return;
}

void exit_ukl(void)
{
       enter_application();
       ukl_handle_signals();
       cond_resched();
}
#else
#define enter_ukl()    do {} while(0)
#define exit_ukl()     do {} while(0)
#endif



void printukl(const char *fmt, ...) {
	static char buf[1024];
	va_list args;
	int len;

	enter_ukl();

	va_start(args, fmt);
	len = vscnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	if (len && buf[len - 1] == '\n')
		buf[len - 1] = '\0';


	printk("%s\n", buf);
	exit_ukl();
}

ssize_t ukl_write(int fd, const void* buf, size_t count) {
	extern ssize_t __ukl_write(int fd, const void* buf, size_t count);
	ssize_t retval;
	enter_ukl();
	retval = __ukl_write(fd, buf, count);
	exit_ukl();
	return retval;
}

ssize_t ukl_read(int fd, const void* buf, size_t count){
	extern ssize_t __ukl_read(int fd, const void* buf, size_t count);
	ssize_t retval;
	enter_ukl();
	retval = __ukl_read(fd, buf, count);
	exit_ukl();
	return retval;
}

long ukl_open(const char* filename, int flags, unsigned short mode){
	extern long __ukl_open(const char* filename, int flags, unsigned short mode);
	long retval;
	enter_ukl();
	retval = __ukl_open(filename, flags, mode);
	exit_ukl();
	return retval;
}

long ukl_openat(int dfd, const char * filename, int flags, umode_t mode){
	extern long __ukl_openat(int dfd, const char * filename, int flags, umode_t mode);
	long retval;
	enter_ukl();
	retval = __ukl_openat(dfd, filename, flags, mode);
	exit_ukl();
	return retval;
}

long ukl_close(int fd){
	extern int __ukl_close(unsigned int fd);
	long retval;
	enter_ukl();
	retval = __ukl_close(fd);
	exit_ukl();
	return retval;
}

void ukl_exit_group(int error_code){
	extern void __ukl_exit_group(int error_code);
	enter_ukl();
	__ukl_exit_group(error_code);
	exit_ukl();
}

int ukl_socket(int family, int type, int protocol){
	extern int __ukl_socket(int family, int type, int protocol);
	int retval;
	enter_ukl();
	retval = __ukl_socket(family, type, protocol);
	exit_ukl();
	return retval;
}

int ukl_bind(int fd, struct sockaddr  *umyaddr, int addrlen){
	extern int __ukl_bind(int fd, struct sockaddr  *umyaddr, int addrlen);
	int retval;
	enter_ukl();
	retval = __ukl_bind(fd, umyaddr, addrlen);
	exit_ukl();
	return retval;
}

int ukl_connect(int fd, struct sockaddr  *uservaddr, int addrlen){
	extern int __ukl_connect(int fd, struct sockaddr  * uservaddr, int addrlen);
	int retval;
	enter_ukl();
	retval = __ukl_connect(fd, uservaddr, addrlen);
	exit_ukl();
	return retval;
}

int ukl_listen(int fd, int backlog){
	extern int __ukl_listen(int fd, int backlog);
	int retval;
	enter_ukl();
	retval = __ukl_listen(fd, backlog);
	exit_ukl();
	return retval;
}

int ukl_accept(int fd, struct sockaddr *upeer_sockaddr, int *upeer_addrlen){
	extern int __ukl_accept(int fd, struct sockaddr *upeer_sockaddr, int *upeer_addrlen);
	int retval;
	enter_ukl();
	retval = __ukl_accept(fd, upeer_sockaddr, upeer_addrlen);
	exit_ukl();
	return retval;
}

int ukl_ioctl(int fd, int cmd, long arg){
	extern int __ukl_ioctl(int fd, int cmd, long arg);
	int retval;
	enter_ukl();
	retval = __ukl_ioctl(fd, cmd, arg);
	exit_ukl();
	return retval;
}

int ukl_recvfrom(int fd, void  *ubuf, size_t size, unsigned int flags, struct sockaddr  *addr, int  *addr_len){
	extern int __ukl_recvfrom(int fd, void  * ubuf, size_t size, unsigned int flags, struct sockaddr  * addr, int  * addr_len);
	int retval;
	enter_ukl();
	retval = __ukl_recvfrom(fd, ubuf, size, flags, addr, addr_len);
	exit_ukl();
	return retval;
}

int ukl_sendto(int fd, void *buff, size_t len, unsigned int flags, struct sockaddr *addr, int addr_len){
	extern int __ukl_sendto(int fd, void *buff, size_t len, unsigned int flags, struct sockaddr *addr, int addr_len);
	int retval;
	enter_ukl();
	retval = __ukl_sendto(fd, buff, len, flags, addr, addr_len);
	exit_ukl();
	return retval;
}

int ukl_recv(int fd, void  *ubuf, size_t size, unsigned int flags){
	extern int __ukl_recv(int fd, void  *ubuf, size_t size, unsigned int flags);
	int retval;
	enter_ukl();
	retval = __ukl_recv(fd, ubuf, size, flags);
	exit_ukl();
	return retval;
}

int ukl_send(int fd, void *buff, size_t len, unsigned int flags){
	extern int __ukl_send(int fd, void *buff, size_t len, unsigned int flags);
	int retval;
	enter_ukl();
	retval = __ukl_send(fd, buff, len, flags);
	exit_ukl();
	return retval;
}

int ukl_setsockopt(int fd, int level, int optname, char *optval, int optlen){
	extern int __ukl_setsockopt(int fd, int level, int optname, char *optval, int optlen);
	int retval;
	enter_ukl();
	retval = __ukl_setsockopt(fd, level, optname, optval, optlen);
	exit_ukl();
	return retval;
}

int ukl_getsockopt(int fd, int level, int optname, char *optval, int * optlen){
	extern int __ukl_getsockopt(int fd, int level, int optname, char *optval, int * optlen);
	int retval;
	enter_ukl();
	retval = __ukl_getsockopt(fd, level, optname, optval, optlen);
	exit_ukl();
	return retval;
}

long ukl_arch_prctl(int option, unsigned long arg2){
	extern long __ukl_arch_prctl(int option, unsigned long arg2);
	long retval;
	enter_ukl();
	retval = __ukl_arch_prctl(option, arg2);
	exit_ukl();
	return retval;
}

int ukl_get_thread_area(struct user_desc  *u_info){
	extern int __ukl_get_thread_area(struct user_desc  * u_info);
	int retval;
	enter_ukl();
	retval = __ukl_get_thread_area(u_info);
	exit_ukl();
	return retval;
}

int ukl_set_thread_area(struct user_desc  *u_info){
	extern int __ukl_set_thread_area(struct user_desc  *u_info);
	int retval;
	enter_ukl();
	retval = __ukl_set_thread_area(u_info);
	exit_ukl();
	return retval;
}

unsigned long ukl_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long off){
	extern unsigned long __ukl_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long off);
	unsigned long retval;
	enter_ukl();
	retval = __ukl_mmap(addr, len, prot, flags, fd, off);
	exit_ukl();
	//printk("MMAP:\taddr = 0x%lx\tlen = 0x%lx\n", retval, len);
	return retval;
}

int ukl_set_tid_address(int * tidptr){
	extern int __ukl_set_tid_address(int * ptr);
	int retval;
	enter_ukl();
	retval = __ukl_set_tid_address(tidptr);
	exit_ukl();
	return retval;
}

int ukl_set_robust_list(struct robust_list_head * head, size_t len){
	extern int __ukl_set_robust_list(struct robust_list_head * head, size_t len);
	int retval;
	enter_ukl();
	retval = __ukl_set_robust_list(head, len);
	exit_ukl();
	return retval;
}

int ukl_rt_sigprocmask(int how, sigset_t * nset,  sigset_t * oset, size_t sigsetsize){
	extern int __ukl_rt_sigprocmask(int how, sigset_t * nset,  sigset_t * oset, size_t sigsetsize);
	int retval;
	enter_ukl();
	retval = __ukl_rt_sigprocmask(how, nset, oset, sigsetsize);
	exit_ukl();
	return retval;
}

int ukl_rt_sigaction(int sig, const struct sigaction * act, struct sigaction * oact, size_t sigsetsize){
	extern int __ukl_rt_sigaction(int sig, const struct sigaction * act, struct sigaction * oact, size_t sigsetsize);
	int retval;
	enter_ukl();
	retval = __ukl_rt_sigaction(sig, act, oact, sigsetsize);
	exit_ukl();
	return retval;
}

int ukl_prlimit64(pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim, struct rlimit64 * old_rlim){
	extern int __ukl_prlimit64(pid_t pid, unsigned int resource, const struct rlimit64 * new_rlim,	struct rlimit64 * old_rlim);
	int retval;
	enter_ukl();
	retval = __ukl_prlimit64(pid, resource, new_rlim, old_rlim);
	exit_ukl();
	return retval;
}


unsigned long ukl_brk(unsigned long brk){
	extern unsigned long __ukl_brk(unsigned long brk);
	unsigned long retval;
	enter_ukl();
	retval = __ukl_brk(brk);
	exit_ukl();
	return retval;
}

int ukl_fstat(unsigned int fd, struct stat  * statbuf){
	extern int __ukl_fstat(unsigned int fd, struct stat  * statbuf);
	int retval;
	enter_ukl();
	retval = __ukl_fstat(fd, statbuf);
	exit_ukl();
	return retval;
}

int ukl_mprotect(unsigned long start, size_t len, unsigned long prot){
	extern int __ukl_mprotect(unsigned long start, size_t len, unsigned long prot);
	int retval;
	enter_ukl();
	retval = __ukl_mprotect(start, len, prot);
	exit_ukl();
	return retval;
}


int ukl_munmap(unsigned long addr, size_t len){
	//printk("UNMAP START:\taddr = 0x%lx\tlen = 0x%lx\n", addr, len);
	extern int __ukl_munmap(unsigned long addr, size_t len);
	int retval;
	enter_ukl();
	retval = __ukl_munmap(addr, len);
	exit_ukl();
	//printk("UNMAP DONE:\taddr = 0x%lx\tlen = 0x%lx\n", addr, len);
	return retval;
}

void ukl_exit(int error_code){
	extern void __ukl_exit(int error_code);
	enter_ukl();
	__ukl_exit(error_code);
	exit_ukl();
}

long ukl_futex(unsigned int  * uaddr, int op, unsigned int  val, struct __kernel_timespec * utime, unsigned int  * uaddr2, unsigned int  val3){
	extern long __ukl_futex(unsigned int  * uaddr, int op, unsigned int  val, struct __kernel_timespec * utime, unsigned int  * uaddr2, unsigned int  val3);
	long retval;
	enter_ukl();
	retval = __ukl_futex(uaddr, op, val, utime, uaddr2, val3);
	exit_ukl();
	return retval;
}

int ukl_setrlimit (unsigned int  resource, const struct rlimit* rlim){
	extern int __ukl_setrlimit(unsigned int resource, const struct rlimit* rlim);
	int retval;
	enter_ukl();
	retval = __ukl_setrlimit(resource, rlim);
	exit_ukl();
	return retval;
}

int ukl_clock_gettime(const clockid_t which_clock, struct __kernel_timespec * tp){
	extern int __ukl_clock_gettime(const clockid_t which_clock, struct __kernel_timespec * tp);
	int retval;
	enter_ukl();
	retval = __ukl_clock_gettime(which_clock, tp);
	exit_ukl();
	return retval;
}

int ukl_gettimeofday(struct timeval* tv, struct timezone* tz){
	extern int __ukl_gettimeofday(struct timeval* tv, struct timezone* tz);
	int retval;
	enter_ukl();
	retval = __ukl_gettimeofday(tv, tz);
	exit_ukl();
	return retval;
}

int ukl_epoll_create1(int flags){
	extern int __ukl_epoll_create1(int flags);
	int retval;
	enter_ukl();
	retval = __ukl_epoll_create1(flags);
	exit_ukl();
	return retval;
}

int ukl_epoll_create(int size){
	extern int __ukl_epoll_create(int size);
	int retval;
	enter_ukl();
	retval = __ukl_epoll_create(size);
	exit_ukl();
	return retval;
}

int ukl_pipe2(int* fildes, int flags){
	extern int __ukl_pipe2(int* fildes, int flags);
	int retval;
	enter_ukl();
	retval = __ukl_pipe2(fildes, flags);
	exit_ukl();
	return retval;
}

time_t ukl_time (time_t *t){
	extern time_t __ukl_time (time_t *t);
	time_t retval;
	enter_ukl();
	retval = __ukl_time (t);
	exit_ukl();
	return retval;
}

int ukl_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg){
	extern int __ukl_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg);
	int retval;
	enter_ukl();
	retval = __ukl_fcntl(fd, cmd, arg);
	exit_ukl();
	return retval;
}

int ukl_epoll_ctl(int epfd, int op, int fd, struct epoll_event* event){
	extern int __ukl_epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
	int retval;
	enter_ukl();
	retval = __ukl_epoll_ctl(epfd, op, fd, event);
	exit_ukl();
	return retval;
}

int ukl_epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout){
	extern int __ukl_epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
	int retval;
	enter_ukl();
	retval =  __ukl_epoll_wait(epfd, events, maxevents, timeout);
	exit_ukl();
	return retval;
}

int ukl_nanosleep(struct __kernel_timespec *rqtp, struct __kernel_timespec *rmtp){
	extern int __ukl_nanosleep(struct __kernel_timespec *rqtp, struct __kernel_timespec *rmtp);
	int retval;
	enter_ukl();
	retval = __ukl_nanosleep(rqtp, rmtp);
	exit_ukl();
	return retval;
}

int ukl_mlock(long start, size_t len){
	extern int __ukl_mlock(long start, size_t len);
	int retval;
	enter_ukl();
	retval = __ukl_mlock(start, len);
	exit_ukl();
	return retval;
}

int ukl_mlock2(long start, size_t len, int flags){
	extern int __ukl_mlock2(long start, size_t len, int flags);
	int retval;
	enter_ukl();
	retval = __ukl_mlock2(start, len, flags);
	exit_ukl();
	return retval;
}

int ukl_tgkill(pid_t tgid, pid_t pid, int sig){
	extern int __ukl_tgkill(pid_t tgid, pid_t pid, int sig);
	int retval;
	enter_ukl();
	retval = __ukl_tgkill(tgid, pid, sig);
	exit_ukl();
	return retval;
}

pid_t ukl_getpid(void){
	extern pid_t __ukl_getpid(void);
	pid_t retval;
	enter_ukl();
	retval = __ukl_getpid();
	exit_ukl();
	return retval;
}

pid_t ukl_gettid(void){
	extern pid_t __ukl_gettid(void);
	pid_t retval;
	enter_ukl();
	retval = __ukl_gettid();
	exit_ukl();
	return retval;
}

int ukl_sendmmsg(int fd, struct mmsghdr *mmsg, unsigned int vlen, unsigned int flags){
	extern int __ukl_sendmmsg(int fd, struct mmsghdr *mmsg, unsigned int vlen, unsigned int flags);
	int retval;
	enter_ukl();
	retval = __ukl_sendmmsg(fd, mmsg, vlen, flags);
	exit_ukl();
	return retval;
}

long ukl_mknod(const char * filename, umode_t mode, unsigned int dev){
	extern long __ukl_mknod(const char * filename, umode_t mode, unsigned int dev);
	long retval;
	enter_ukl();
	retval = __ukl_mknod(filename, mode, dev);
	exit_ukl();
	return retval;
}

int ukl_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data){
	extern int __ukl_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data);
	int retval;
	enter_ukl();
	retval =__ukl_mount(dev_name, dir_name, type, flags, data);
	exit_ukl();
	return retval;
}

int ukl_chroot(const char * filename){
	extern int __ukl_chroot(const char * filename);
	int retval;
	enter_ukl();
	retval = __ukl_chroot(filename);
	exit_ukl();
	return retval;
}

int ukl_chdir(const char * filename){
	extern int __ukl_chdir(const char * filename);
	int retval;
	enter_ukl();
	retval = __ukl_chdir(filename);
	exit_ukl();
	return retval;
}

long ukl_mkdir(const char *pathname, umode_t mode){
	extern long __ukl_mkdir(const char *pathname, umode_t mode);
	int retval;
	enter_ukl();
	retval = __ukl_mkdir(pathname, mode);
	exit_ukl();
	return retval;
}

void ukl_sync(void){
	extern void __ukl_sync(void);
	enter_ukl();
	__ukl_sync();
	exit_ukl();
	return;
}

int ukl_select(int n, fd_set  * inp, fd_set  * outp, fd_set  * exp, struct __kernel_old_timeval  * tvp){
	extern int __ukl_select(int n, fd_set  * inp, fd_set  * outp, fd_set  * exp, struct __kernel_old_timeval  * tvp);
	int retval;
	enter_ukl();
	retval = __ukl_select(n, inp, outp, exp, tvp);
	exit_ukl();
	return retval;
}

int ukl_poll(struct pollfd * ufds, unsigned int nfds, int timeout_msecs){
	extern int __ukl_poll(struct pollfd * ufds, unsigned int nfds, int timeout_msecs);
	int retval;
	enter_ukl();
	retval =__ukl_poll(ufds, nfds, timeout_msecs);
	exit_ukl();
	return retval;
}

int ukl_sysinfo(struct sysinfo * info){
	extern int __ukl_sysinfo(struct sysinfo * info);
	int retval;
	enter_ukl();
	retval =__ukl_sysinfo(info);
	exit_ukl();
	return retval;
}


