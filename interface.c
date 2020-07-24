#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
// #include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <net/sock.h>
#include <net/tcp_states.h>

#include <linux/ukl.h>
#include <linux/kmain.h>
#include <asm/sections.h>
#include <asm/proto.h>
#include <asm/fsgsbase.h>
#include <linux/sched.h>

void * tls;

extern void __libc_setup_tls (unsigned long start, unsigned long tbss_start, unsigned long end);
extern void __ctype_init (void);
extern unsigned long get_gs_value(void);
extern int fsbringup(void);
extern void ukl_sync(void);

void splash_screen(void);

void printaddr(long addr){
	printk("*** Called from addr 0x%lx ***\n", addr);
}

unsigned int inet_addr2(char* ip)
{
    int a, b, c, d;
    char addr[4];
    
    sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
    addr[0] = a;
    addr[1] = b;
    addr[2] = c;
    addr[3] = d;
    
    return *(unsigned int *)addr;
}

void __copy_tls(void * dest, void * src, size_t n, size_t m){
	memset (memcpy (dest, src, n) + n, '\0', m);
	return;
}

void setup_mm(void){
    struct task_struct *me = current;

    printk("me aka current is %px\n", me);
    printk("me->mm aka current->mm is %px\n", me->mm);

    // Allocate an mm
    me->mm = mm_alloc();
    // hmm
    // In gdb (): 0xffffffff81021ae0
    me->mm->get_unmapped_area = arch_get_unmapped_area_topdown;

    // In gdb: 0x7fefa59bd000
    // 1 tb down from  top of USER address space
    me->mm->mmap_base = 0x7f0000000000; 

    // In gdb
    // center user space
    me->mm->mmap_legacy_base = 0x300000000000;

    // One pg from END of userspace
    // this probably breaks with lots of env vars.
    me->mm->task_size = 0x7ffffffff000; // last page
    // Think a pg is stolen by kernel for argvc and envts etc

    // Real low in USR addr space
    me->mm->start_brk = 0x405000;
    // Same number
    me->mm->brk = 0x405000;

    me->active_mm = me->mm;

    printk("thread_info->flags = %lx\n", me->thread_info.flags);
    printk("Set up of mm struct, done.\n");
}

void lib_start_kmain(void){
    struct task_struct *me = current;
    printk("%s setting up tls\n", __FUNCTION__);

    // None of these args are being used. Linker would have picked this up if we didn't abuse it.
    __libc_setup_tls(__tls_data_start, __tls_bss_start, __tls_bss_end);
    printk("TLS address for main thread is %lx\n", me->thread.fsbase);
    __pthread_initialize_minimal_internal(me->thread.fsbase);
    printk("Set up of TCB done. \n");
    __ctype_init();
    printk("Set up of ctype data done. \n");

    printk("Old task struct flags = %x\n", me->flags);

    // This is the value before calling interface
    // this is what's different from when running pthread
    /* 0x2---4- */ // What's different from his
    // fork no exec
    // k thread

    /* 0x404100 */ // ours 

    // This is the value while running the app
    /* 0x404100 */
    /*   PF_SUPERPRIV */
    /*   PF_USED_ASYNC */
    /*   PF_RANDOMIZE */   // probably don't want
    /*   me->flags = me->flags^PF_KTHREAD; */   // missing this one
    /*   me->flags = me->flags^PF_NOFREEZE; */  // missing this one
/* #define PF_RANDOMIZE		0x00400000	/\* Randomize virtual address space *\/ */
/* #define PF_KTHREAD		0x00200000	/\* I am a kernel thread *\/ */
/* #define PF_NOFREEZE		0x00008000	/\* This thread should not be frozen *\/ */


    // ****************************
    // Top of  run_init_process
    // On classic path: 0x204140 (same as init)
    // fork_no_exec is on
    // kthread is on
    // used_asynch is on
    // superpriv is on

    // New kthread running interface() flags: 0x208040
    // Differs from last thread by
    // superpriv is off
    // used asynch is off
    // nofreeze is on

    // Setup code here flips 4 flags: 
    /* KTHREAD;  flips this to off */      // This differs from primordial thread
    /* NOFREEZE;   flips this to off */
    /* USED_ASYNC; flips this to on*/
    /* SUPERPRIV;  flips to on */


    // Enter kmain with with: 
    // ****************************

    me->flags = me->flags^PF_KTHREAD;
    me->flags = me->flags^PF_NOFREEZE;
    me->flags = me->flags^PF_USED_ASYNC;
    me->flags = me->flags^PF_SUPERPRIV; 

    printk("Current task struct flags = %x\n", me->flags);
    printk("Current task struct address = %lx\n", me);
    printk("Old task struct thread_info flags = %x\n", me->thread_info.flags);
    me->thread_info.flags = 0;
    printk("Old task struct thread_info flags = %x\n", me->thread_info.flags);
}

void setup_networking(void){
    int fd = -1;
    int retioctl = -1;

    struct sockaddr_in sin; 
    struct ifreq ifr;

    fd = ukl_socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if(fd < 0){
        printk("Problem with socket\n");
        return  -1;
    }

    strncpy(ifr.ifr_name, "eth0\0", 5);
    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;
    sin.sin_port=htons(0);
    sin.sin_addr.s_addr = inet_addr2("10.0.2.15");
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

    retioctl = ukl_ioctl(fd, SIOCSIFADDR, &ifr);
    if(retioctl < 0){
        printk("1st Ioctl failed\n");
        return  -1;
    }


    /*strncpy(ifr.ifr_name, "eth0", 4);*/
    ifr.ifr_flags |= IFF_BROADCAST;
    ifr.ifr_flags |= IFF_MULTICAST;

    retioctl = ukl_ioctl(fd, SIOCGIFFLAGS, &ifr);
    if(retioctl < 0){
        printk("2nd Ioctl failed\n");
        return  -1;
    }


    /*strncpy(ifr.ifr_name, "eth0", 4);*/
    ifr.ifr_flags |= IFF_UP;
    /*ifr.ifr_flags |= IFF_BROADCAST;*/
    ifr.ifr_flags |= IFF_RUNNING;
    /*ifr.ifr_flags |= IFF_MULTICAST;*/

    retioctl = ukl_ioctl(fd, SIOCSIFFLAGS, &ifr);
    if(retioctl < 0){
        printk("3rd Ioctl failed\n");
        return  -1;
    }
#ifndef CONFIG_PREEMPT
#ifdef CONFIG_UNIKERNEL_LINUX
    enter_ukl();
#endif
#endif
    msleep(3000);

#ifndef CONFIG_PREEMPT
#ifdef CONFIG_UNIKERNEL_LINUX
    exit_ukl();
#endif
#endif

    printk("Set up of network interface, done.\n");
}

// Bold Cyan
#define UU "\033[1;36m"
// Bold Magenta
#define KK "\033[1;35m"
// Bold RED
#define LL "\033[1;31m"
// Bold Blue
#define BG "\033[0;34m"
// Reset
#define RS "\033[0m"

void splash_screen(void){
  // Give things a chance to settle down.
  msleep(300);

  // I don't know why this can't be done in a single string.
  char *ukl_splash =
  BG"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" \
  BG" ~~"UU"/\\\\\\"BG"~~~~~~~~"UU"/\\\\\\"BG"~~"      KK"/\\\\\\"BG"~~~~~~~~"KK"/\\\\\\"BG"~~"  LL"/\\\\\\"BG"~~~~~~~~~~~~~\n" \
    "  ~"UU"\\/\\\\\\"BG"~~~~~~~"UU"\\/\\\\\\"BG"~"    KK"\\/\\\\\\"BG"~~~~~"KK"/\\\\\\//"BG"~~" LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n" \
    "   ~"UU"\\/\\\\\\"BG"~~~~~~~"UU"\\/\\\\\\"BG"~"   KK"\\/\\\\\\"BG"~~"KK"/\\\\\\//"BG"~~~~~" LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n" \
    "    ~"UU"\\/\\\\\\"BG"~~~~~~~"UU"\\/\\\\\\"BG"~"  KK"\\/\\\\\\\\\\\\//\\\\\\"BG"~~~~~"      LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n" \
    "     ~"UU"\\/\\\\\\"BG"~~~~~~~"UU"\\/\\\\\\"BG"~" KK"\\/\\\\\\//"BG"~"KK"\\//\\\\\\"BG"~~~~"LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n" \
    "      ~"UU"\\/\\\\\\"BG"~~~~~~~"UU"\\/\\\\\\"BG"~"KK"\\/\\\\\\"BG"~~~~"KK"\\//\\\\\\"BG"~~~"LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n";
  char *ukl_splash2 =
    "       ~"UU"\\//\\\\\\"BG"~~~~~~"UU"/\\\\\\"BG"~~"KK"\\/\\\\\\"BG"~~~~~"KK"\\//\\\\\\"BG"~~"LL"\\/\\\\\\"BG"~~~~~~~~~~~~~\n";
  char *ukl_splash3 =
    "        ~~"UU"\\///\\\\\\\\\\\\\\\\\\/"BG"~~~"    KK"\\/\\\\\\"BG"~~~~~~"KK"\\//\\\\\\"BG"~"LL"\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\"BG"~~\n" \
    "         ~~~~"UU"\\/////////"BG"~~~~~"            KK"\\///"BG"~~~~~~~~"KK"\\///"BG"~~"      LL"\\//////////////"BG"~~~\n" \
    "          ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  printk("%s", ukl_splash);
  printk("%s", ukl_splash2);
  printk("%s"RS"\n", ukl_splash3);
}


#if 0
// rdi rsi rdx rcx r8 r9
/* int *() (int, char * *, char * *) = (*() (int, char * *, char * *)) kmain; */


/* 0xff0 */

/* rsp */
/* 0b....-1111-1111-0000 */

/* ****-****-****-0000 */
/* push rax */
/* rsp ****-****-****-1000 // not 16 byte aligned */
/* push rsp */
/* rsp ****-****-****-0000 // 16 byte aligned */
extern int __libc_start_main(
                             /* int *(main) (int, char * *, char * *), // */
                             void (*main) (void), //
                             int argc,
                             char * * ubp_av,
                             void (*init) (void),
                             void (*fini) (void),
                             void (*rtld_fini) (void),
                             void (* stack_end)
                             );
/* extern void __libc_start_main(void); */
extern void __libc_csu_init(void);
extern void __libc_csu_fini(void);
extern void __libc_csu_fini(void);

void bogus_rtld_fini(void){}

uint64_t argc = 4;
char *argv_0 = "./program";
char *argv_1 = "arg1";
char *argv_2 = "arg2";
char *argv_3 = "arg3";

char *env_0 = "ENV_0=0";
char *env_1 = "ENV_1=1";
char *env_2 = "ENV_2=2";

int tu_start(void);

#define NUM_VECS 48

// This is a copy of the stack values from a gdb dump of a normal linux app.
// Lower on the screen is deeper on the stack (higher addresses in virtual
// address space). stack_aux_vecs[0] is the last aux vec, which should be
// later will come more pushes including a null, then the env vars, then
// a null, then argv finally argc and we're done.
uint64_t stack_aux_vecs[NUM_VECS] = {
                                     0x0000000000000021, 0x00007ffd3a5a3000, //aux vec0, last ones to be pushed
                                     0x0000000000000010, 0x00000000078afbfd, // don't forget ptrs here are bs
                                     0x0000000000000006, 0x0000000000001000,
                                     0x0000000000000011, 0x0000000000000064,
                                     0x0000000000000003, 0x0000000000400040,
                                     0x0000000000000004, 0x0000000000000038,
                                     0x0000000000000005, 0x0000000000000008,
                                     0x0000000000000007, 0x0000000000000000,
                                     0x0000000000000008, 0x0000000000000000,
                                     0x0000000000000009, 0x0000000000401ab0,
                                     0x000000000000000b, 0x0000000000000000,
                                     0x000000000000000c, 0x0000000000000000,
                                     0x000000000000000d, 0x0000000000000000,
                                     0x000000000000000e, 0x0000000000000000,
                                     0x0000000000000017, 0x0000000000000000,
                                     0x0000000000000019, 0x00007ffd3a4c3119,
                                     0x000000000000001a, 0x0000000000000000,
                                     0x000000000000001f, 0x00007ffd3a4c3fed,
                                     0x000000000000000f, 0x00007ffd3a4c3129, // last aux vec
                                     0x0000000000000000, 0x0000000000000000, // Up to 16 bytes padding?
                                     0x0000000000000000, 0x505270e3789c2900, // argument ASCIIZ strings?
                                     0xb95ce534f44bfdb2, 0x0034365f3638787d, // environment ASCIIZ str?
                                     0x0000000000000000, 0x0000000000000000, // Bunch of 0s because that seems right.
                                     0x0000000000000000, 0x0000000000000000  // <- This is the first to be pushed.
                                     };

int tu_start(){
  // rdi rsi rdx rcx r8 r9

  // Start at end and push each.
  /* int i=NUM_VECS-1; */
  /* for (; i>=0; i--){ */
  /*   asm ("pushq %0" : : "r"(stack_aux_vecs[i]) ); */
  /* } */


  asm(
    /* "pushq $0 \n\t" // null separating envs from aux vecs */
    /* "push env_2\n\t" */
    /* "push env_1\n\t" */
    /* "push env_0\n\t" */
    /* "pushq $0  \n\t" // null separating envs from argvs */
    /* "push argv_3 \n\t" */
    /* "push argv_2 \n\t" */
    /* "push argv_1 \n\t" */
    /* "push argv_0 \n\t" */
    /* "push   argc \n\t" // argc */

    "xor    %ebp,%ebp \n\t"
    "mov    $0,%r9 \n\t" // rtld_fini (was 0 on linux)
    "pop    %rsi \n\t" // argc
    "mov    %rsp,%rdx \n\t" // ptr to argv[0]
    "and    $0xfffffffffffffff0,%rsp \n\t" // 16 byte align
    "mov    $0,%rax \n\t" // Just push 0 instead of garbage
    "push   %rax \n\t" // Filler?
    "push   %rsp \n\t" // Don't really understand. 
    "mov    __libc_csu_fini,%r8 \n\t" // arg 7
    "mov    __libc_csu_init,%rcx \n\t" // arg 4
    "mov    kmain,%rdi \n\t" // arg 1
    "callq  __libc_start_main \n\t"
    /* "hlt \n\t" */
    /* "nopw   0x0(%rax,%rax,1)" */
      );
  return 0;
  }
int main(int argc, char *argv[]);
#endif


void set_flags_shit(void);

void set_flags_shit(){
  struct task_struct *me = current;

  /* #define PF_KTHREAD		0x00200000	/\* I am a kernel thread *\/ */
  /* #define PF_NOFREEZE		0x00008000	/\* This thread should not be frozen *\/ */

  // of course we have execed, but want to match ali's work.
/* #define PF_FORKNOEXEC		0x00000040	/\* Forked but didn't exec *\/ */

  // ****************************
  // On init path: 0x204140
  // Before exec, 4 flags are set:
  // fork_no_exec is on
  // kthread is on
  // used_asynch is on
  // superpriv is on

  // exec flips 3 flags: 0x404100
  // fork_no_exec turns off
  // randomize turns on
  // kthread turns off.

  // in interface init path flips 3 flags: 
  // randomize turned off
  // kthread turned on
  // nofreeze turned on

  // Enter program with: 0x20c100
  // ****************************

  // Disable randomize
  /* #define PF_RANDOMIZE		0x00400000	/\* Randomize virtual address space *\/ */
  me->flags &= ~PF_RANDOMIZE;

  // Set fork no exec, it's a lie, but in keeping with how things were done.
  /* #define PF_FORKNOEXEC		0x00000040	/\* Forked but didn't exec *\/ */
  me->flags |= PF_RANDOMIZE;


  me->thread_info.flags = 0;
}

extern void _start(void);

/* #define UKL_INT_PATH */
int interface(void)
{
  printk("Running interface!!\n");

  setup_networking();
  /* set_flags_shit(); */
  fsbringup();

  return 0;
}
