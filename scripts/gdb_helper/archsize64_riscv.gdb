
# ==============================================================================
# RISC-V 64 SPECIFIC HARDWARE RULES & MACROS
# ==============================================================================

# --- Auto-Display General Purpose Registers ---
define set_disp_riscv_regs
  # Core execution pointers
  display /r $ra
  display /r $sp
  display /r $fp
  display /r $tp
  
  # First 4 arguments usually hold the most context
  display /r $a0
  display /r $a1
  display /r $a2
  display /r $a3
  
  # Critical Supervisor State
  display /r $sstatus
  display /r $sepc
  display /r $scause
end


# ==============================================================================
# RISC-V SCAUSE DECODER (Complete Python Implementation)
# ==============================================================================
python
import gdb

class DecodeScause(gdb.Command):
    """Decodes the RISC-V scause register into human-readable text."""
    
    def __init__(self):
        super(DecodeScause, self).__init__("decode_scause", gdb.COMMAND_USER)
        
        # The Complete RISC-V Exception Code Dictionary
        self.exceptions = {
            0: "Instruction Address Misaligned",
            1: "Instruction Access Fault (Physical Memory Blocked)",
            2: "Illegal Instruction",
            3: "Breakpoint (EBREAK)",
            4: "Load Address Misaligned",
            5: "Load Access Fault (Physical Memory Blocked)",
            6: "Store/AMO Address Misaligned",
            7: "Store/AMO Access Fault (Physical Memory Blocked)",
            8: "Environment Call (Syscall) from U-Mode",
            9: "Environment Call from S-Mode",
            11: "Environment Call from M-Mode", 
            12: "Instruction Page Fault (MMU Blocked)",
            13: "Load Page Fault (MMU Blocked / NULL Dereference)",
            15: "Store/AMO Page Fault (MMU Blocked / Read-Only Write)",
            
            # Hypervisor / Virtualization Extensions
            20: "Instruction Guest-Page Fault",
            21: "Load Guest-Page Fault",
            22: "Virtual Instruction",
            23: "Store/AMO Guest-Page Fault"
        }
        
        # The Complete RISC-V S-Mode Interrupt Dictionary
        self.interrupts = {
            1: "Supervisor Software Interrupt (IPI)",
            5: "Supervisor Timer Interrupt",
            9: "Supervisor External Interrupt (PLIC/AIA)",
            13: "Counter Interrupt" 
        }

    def invoke(self, arg, from_tty):
        try:
            scause_val = int(gdb.parse_and_eval("$scause"))
        except gdb.error:
            print("Error: Could not read $scause.")
            return

        is_interrupt = (scause_val >> 63) & 1
        ecode = scause_val & 0x7FFFFFFFFFFFFFFF

        if is_interrupt:
            reason = self.interrupts.get(ecode, "Unknown/Reserved Interrupt ({})".format(ecode))
        else:
            reason = self.exceptions.get(ecode, "Unknown/Reserved Exception ({})".format(ecode))

        print("SCAUSE Decode: [0x{:016x}] -> {}".format(scause_val, reason))

DecodeScause()
end


# --- Quick Info General Registers ---
define infr
    # Added Stack Pointer (sp), Frame Pointer (fp), Thread Pointer (tp), Global (gp)
    info registers $ra $sp $fp $tp $gp
    info registers $a0 $a1 $a2 $a3 $a4 $a5 $a6 $a7
    info registers $t0 $t1 $t2 $t3 $t4 $t5 $t6
end

# --- Quick Info Exception/System Registers (S-Mode) ---
define info_supervisor_reg
    echo [\n] Supervisor Mode (S-Mode) CSRs [Linux Kernel State]:\n
    
    # scause: Why it trapped (e.g., Page Fault, Syscall)
    # sepc: Where the program counter was when it trapped
    # stval: The bad virtual address (Crucial for MMU debugging)
    # stvec: Where the kernel trap handler is located
    # sstatus: Interrupt enable bits and previous privilege mode
    # satp: Supervisor Address Translation and Protection (Page Table Base)
    info registers scause sepc stval stvec sstatus satp
end

# --- Quick Info Exception/System Registers (M-Mode) ---
define info_machine_reg
    echo [\n] Machine Mode (M-Mode) CSRs [Firmware/SBI State]:\n
    info registers mcause mepc mtval mtvec mstatus mie
end

# --- Architecture Specific Stop Hook ---
define archsize64_riscv-stop-hook
    echo \n=======================================================\n
    echo [RISC-V 64] CPU Paused. Context:\n
    #display /i $pc
    
    # Dump S-Mode registers by default since we are debugging Linux
    info_supervisor_reg
    infr
    
    echo =======================================================\n
end




