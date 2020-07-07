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

#include <asm/sections.h>
#include <asm/proto.h>
#include <asm/fsgsbase.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>

extern void __libc_start_main(void (*main) (void));
extern void kmain(void);

void setup_mm(void){
    struct task_struct *me = current;

   printk("init pgd = 0x%lx\n", init_mm.pgd);   
   printk("current pgd = 0x%lx\n", me->active_mm->pgd);   
   // me->mm = me->active_mm;
    me->mm = mm_alloc();
    me->mm->get_unmapped_area = arch_get_unmapped_area_topdown;
    me->mm->mmap_base = 0x7f0000000000; 
    me->mm->mmap_legacy_base = 0x300000000000;
    me->mm->task_size = 0x7ffffffff000;
    me->mm->start_brk = 0x405000;
    me->mm->brk = 0x405000;

   printk("new pgd = 0x%lx\n", me->mm->pgd);   
    //me->active_mm = me->mm;
    init_mm.pgd = me->mm->pgd;
   printk("init pgd = 0x%lx\n", init_mm.pgd);   

   printk("sem->owner = 0x%lx sem->count = 0x%lx\n", &me->mm->mmap_sem.owner, &me->mm->mmap_sem.count);

    printk("thread_info->flags = %lx\n", me->thread_info.flags);

    printk("Set up of mm struct, done.\n");
}

int interface(void)
{
    setup_mm();

     struct task_struct *me = current;
     printk("Old task struct flags = %x\n", me->flags);
     me->flags = me->flags^PF_KTHREAD;
     me->flags = me->flags^PF_NOFREEZE;
     me->flags = me->flags^PF_USED_ASYNC;
     me->flags = me->flags^PF_SUPERPRIV;
     printk("Current task struct flags = %x\n", me->flags);
     printk("Current task struct address = %lx\n", me);
     printk("Old task struct thread_info flags = %x\n", me->thread_info.flags);
     me->thread_info.flags = 0;
     printk("Old task struct thread_info flags = %x\n", me->thread_info.flags);
    
     __libc_start_main((void *) kmain);

    //kmain();
    
    while(1){
        current->state = TASK_INTERRUPTIBLE;
        schedule();
       }
    
    return 0;
}
