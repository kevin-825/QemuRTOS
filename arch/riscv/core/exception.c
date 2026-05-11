
#include <stdio.h>
#include <stdint.h>
#include "exception.h"
#include "riscv_cpu.h"
#include "encoding.h"
#include "kernel/printk.h"
#define MAX_ECALL_NUM 128
//#include "coredump.h"

uint32_t g_exception_nest_cnt=0;
uint32_t g_current_stack_pointer;
uint32_t g_critical_nesting_cnt;
uint32_t *p_critical_nesting_cnt = &g_critical_nesting_cnt;

long ecall_handler[MAX_ECALL_NUM]={0};

uint32_t xISRStackTop[1024] __attribute__((aligned(16))); // Define an ISR stack

const char *mcause_messages[] = {
    "Instruction address misaligned",
    "Instruction access fault",
    "Illegal instruction",
    "Breakpoint",
    "Load address misaligned",
    "Load access fault",
    "Store/AMO address misaligned",
    "Store/AMO access fault",
    "Environment call from U-mode",
    "Environment call from S-mode",
    "Environment call from VS-mode",
    "Environment call from M-mode",
    "Instruction page fault",
    "Load page fault",
    "Store/AMO page fault",
    "Unknown cause"
};


void print_mcause(uint32_t mcause) {
    if (mcause >= 0 && mcause < 15) {
        printk("%s\n", mcause_messages[mcause]);
    } else {
        printk("%s\n", mcause_messages[15]); // Unknown cause
    }
}

void exception_show_exc_stack_frame_registers(uint32_t* stack_frame) 
{
    uint32_t mcause = read_csr(mcause);
    uint32_t mepc = read_csr(mepc);
    uint32_t mtval = read_csr(mtval);
    uint32_t mstatus = read_csr(mstatus);
    uint32_t mtvec = read_csr(mtvec);
    uint32_t mip = read_csr(mip);
    uint32_t mie = read_csr(mie);

    // Print the exception-related registers
    printk("Exception occurred! machine register info:\n");
    printk("mcause: 0x%08x   ", (unsigned int)mcause);
    print_mcause(mcause);
    printk("mepc: 0x%08x\n", (unsigned int)mepc);
    printk("mtval: 0x%08x\n", (unsigned int)mtval);
    printk("mstatus: 0x%08x\n", (unsigned int)mstatus);
    printk("mtvec: 0x%08x\n", (unsigned int)mtvec);
    printk("mip: 0x%08x\n", (unsigned int)mip);
    printk("mie: 0x%08x\n\n", (unsigned int)mie);
    //printk("tp: 0x%08x\n", stack_frame[IDX_X4_TP]);
    //printk("gp: 0x%08x\n", stack_frame[IDX_X3_GP]);
    
    // Print the register stack frame
    //printk("pvParameters: 0x%08x\n", stack_frame[IDX_PVPARAMETERS]);
    register void *gp_value __asm__("gp");
    register void *tp_value __asm__("tp");
    printk("Exception StackFrame Info:\n");
    printk("sp: 0x%08x\n", (unsigned int)stack_frame);
    printk("gp: 0x%08x\n", (unsigned int)gp_value);
    printk("tp: 0x%08x\n", (unsigned int)tp_value);
    printk("xCriticalNesting: 0x%08x\n", (unsigned int)stack_frame[IDX_XCRITICALNESTING]);
    printk("mstatus: 0x%08x\n", (unsigned int)stack_frame[IDX_MSTATUS]);
    printk("mepc: 0x%08x\n", (unsigned int)stack_frame[IDX_MEPC_ADDED_4]);
        // Print ra
    printk("ra: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X1_RA]);
    // Print t0-t6
    printk("t0: 0x%08x\n", (unsigned int)stack_frame[IDX_X5_T0]);
    printk("t1: 0x%08x\n", (unsigned int)stack_frame[IDX_X6_T1]);
    printk("t2: 0x%08x\n", (unsigned int)stack_frame[IDX_X7_T2]);
    printk("t3: 0x%08x\n", (unsigned int)stack_frame[IDX_X28_T3]);
    printk("t4: 0x%08x\n", (unsigned int)stack_frame[IDX_X29_T4]);
    printk("t5: 0x%08x\n", (unsigned int)stack_frame[IDX_X30_T5]);
    printk("t6: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X31_T6]);
    
    // Print a0-a7
    printk("a0: 0x%08x\n", (unsigned int)stack_frame[IDX_X10_A0]);
    printk("a1: 0x%08x\n", (unsigned int)stack_frame[IDX_X11_A1]);
    printk("a2: 0x%08x\n", (unsigned int)stack_frame[IDX_X12_A2]);
    printk("a3: 0x%08x\n", (unsigned int)stack_frame[IDX_X13_A3]);
    printk("a4: 0x%08x\n", (unsigned int)stack_frame[IDX_X14_A4]);
    printk("a5: 0x%08x\n", (unsigned int)stack_frame[IDX_X15_A5]);
    #ifndef __riscv_32e
        printk("a6: 0x%08x\n", (unsigned int)stack_frame[IDX_X16_A6]);
        printk("a7: 0x%08x\n\n", (unsigned int)stack_frame[IDX_X17_A7]);
    #endif /* ifndef __riscv_32e */
    
    // Print s0-s11
    printk("s0/fp: 0x%08x\n", (unsigned int)stack_frame[IDX_X8_S0_FP]);
    printk("s1: 0x%08x\n", (unsigned int)stack_frame[IDX_X9_S1]);
    #ifndef __riscv_32e
        printk("s2: 0x%08x\n", (unsigned int)stack_frame[IDX_X18_S2]);
        printk("s3: 0x%08x\n", (unsigned int)stack_frame[IDX_X19_S3]);
        printk("s4: 0x%08x\n", (unsigned int)stack_frame[IDX_X20_S4]);
        printk("s5: 0x%08x\n", (unsigned int)stack_frame[IDX_X21_S5]);
        printk("s6: 0x%08x\n", (unsigned int)stack_frame[IDX_X22_S6]);
        printk("s7: 0x%08x\n", (unsigned int)stack_frame[IDX_X23_S7]);
        printk("s8: 0x%08x\n", (unsigned int)stack_frame[IDX_X24_S8]);
        printk("s9: 0x%08x\n", (unsigned int)stack_frame[IDX_X25_S9]);
        printk("s10: 0x%08x\n", (unsigned int)stack_frame[IDX_X26_S10]);
        printk("s11: 0x%08x\n", (unsigned int)stack_frame[IDX_X27_S11]);
    #endif /* ifndef __riscv_32e */
    
}






void risc_v_application_exception_handler( uint32_t ulMcause )
{
    riscv_cpu_interrupt_global_disable();
    ( void ) ulMcause;

	print_mcause(ulMcause);
    //extern TaskHandle_t pxCurrentTCB;
    uint32_t *pxCurrentTCB=(uint32_t *)0;
    uint32_t *p_exc_stac_frame=(uint32_t *)0;
    if (pxCurrentTCB) {
        p_exc_stac_frame = (uint32_t *)(*(uint32_t *)pxCurrentTCB);
    } else {
        p_exc_stac_frame = (uint32_t *)g_current_stack_pointer;
    }

    exception_show_exc_stack_frame_registers(p_exc_stac_frame);



    //CpuContext context;

    // Capture CPU context (replace with actual retrieval code)
    //memset(&context, 0, sizeof(CpuContext));
    //context.mcause = 0; // Replace with actual cause
    //context.mepc = 0;   // Replace with actual program counter

    //serialize_and_dump(&context);
    while(1){
        
	}
}

void risc_v_ecall_handler( uint32_t ulMcause )
{
    register long syscall_num asm("a7");
    register long arg0 asm("a0");
    register long arg1 asm("a1");
    register long arg2 asm("a2");
    register long arg3 asm("a3");
    register long arg4 asm("a4");
    register long arg5 asm("a5");
    register long arg6 asm("a6");
    printk("risc_v_ecall_handler ulMcause:0x%x \n", (unsigned int)ulMcause);
    printk("Syscall number: %ld\n", syscall_num);
    printk("Arg0: %ld, Arg1: %ld, Arg2: %ld, Arg3: %ld, Arg4: %ld, Arg5: %ld, Arg6: %ld\n",
           arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    
    if (syscall_num >=0 && syscall_num < MAX_ECALL_NUM && ecall_handler[syscall_num]!=0) {
        long ret = ((long (*)(long, long, long, long, long, long, long))ecall_handler[syscall_num])(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        // Return value in a0
        asm volatile ("mv a0, %0" : : "r"(ret));
    } else {
        printk("Unhandled syscall number: %ld\n", syscall_num);
        // Handle unregistered syscall, e.g., return -1
        asm volatile ("li a0, -1"); 
    }

    

}

void register_ecall_handler(long syscall_num, ecall_handler_t handler) 
{
    if (syscall_num >=0 && syscall_num < MAX_ECALL_NUM) {
        ecall_handler[syscall_num] = (long)handler;
    }
}





