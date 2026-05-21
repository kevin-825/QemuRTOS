/* kernel/fatal.c */
#include "kernel/fatal.h"
#include "kernel/irq.h"
#include "kernel/printk.h"
/* Temporary tracker until trap.S and exception.c are fully mapped */
volatile int interrupt_nesting_count = 0;



static const char *reason_to_str(unsigned int reason)
{
	switch (reason) {
	case K_ERR_CPU_EXCEPTION:
		return "CPU exception";
	case K_ERR_SPURIOUS_IRQ:
		return "Unhandled interrupt";
	case K_ERR_STACK_CHK_FAIL:
		return "Stack overflow";
	case K_ERR_KERNEL_OOPS:
		return "Kernel oops";
	case K_ERR_KERNEL_PANIC:
		return "Kernel panic";
	default:
		return "Unknown error";
	}
}


void kernel_fatal_error(unsigned int reason, const struct arch_esf *esf)
{
    unsigned int key = arch_irq_lock();
    printk("\n===================================================================\n");
    printk(" 🚨 KERNEL PANIC 🚨\n");
    printk("===================================================================\n");
    printk("System Halted.\n");
    printk("===================================================================\n");

    arch_halt(); /* Sleep forever */

    //coredump(reason, esf, thread);


    while(1){}

    printk("Action: Terminating faulting thread and rescheduling...\n");
    printk("===================================================================\n");

    /* * TODO: Once the Context Switcher (Scheduler) is built, uncomment this:
     * * thread_abort(_current_thread);
     * schedule(); 
     * */
};
