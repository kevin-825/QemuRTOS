# ==============================================================================
# ARM64 SPECIFIC HARDWARE RULES & MACROS (PURE GDB VERSION)
# ==============================================================================
#set architecture aarch64

# --- 2. Quick Info Registers ---
define infr
    echo [\n] General Purpose Registers:\n
    info registers x30 x29
    info registers x0 x1 x2 x3 x4 x5 x6 x7
    info registers x9 x10 x11 x12 x13 x14 x15
    info registers sp pc cpsr
end

define info_sys_reg
    echo [\n] Exception Level 1 (EL1) System Registers [Linux State]:\n
    info registers ESR_EL1 ELR_EL1 FAR_EL1 VBAR_EL1 SPSR_EL1
end

# --- Exception Level 2 (Hypervisor) Registers ---
define info_el2_reg
    echo [\n] Exception Level 2 (EL2) System Registers [Hypervisor State]:\n
    # Changed to ALL CAPS
    info registers ESR_EL2 ELR_EL2 FAR_EL2 VBAR_EL2 SPSR_EL2
end

# --- Exception Level 3 (Secure Monitor) Registers ---
define info_el3_reg
    echo [\n] Exception Level 3 (EL3) System Registers [Firmware/TrustZone State]:\n
    # Changed to ALL CAPS
    info registers ESR_EL3 ELR_EL3 FAR_EL3 VBAR_EL3 SPSR_EL3
end

# --- Current Privilege Level Check ---
define current_el
    # The cpsr register holds the Exception Level in bits [3:2]
    set $el = ($cpsr >> 2) & 3
    printf "CPU is currently executing in: EL%d\n", $el
end

# --- 4. Native GDB Exception Decoder (Python-Free) ---
define decode_esr
    # Extract Exception Class (EC) from bits [31:26]
    set $ec = ($ESR_EL1 >> 26) & 0x3F
    
    if ($ec == 0x15)
        printf "ESR_EL1 Decode: [0x%08x] -> SVC instruction execution (Syscall)\n", $ESR_EL1
    end
    if ($ec == 0x20)
        printf "ESR_EL1 Decode: [0x%08x] -> Instruction Abort from a lower Exception Level\n", $ESR_EL1
        printf "FAR_EL1 (Faulting Address): 0x%016x\n", $FAR_EL1
    end
    if ($ec == 0x21)
        printf "ESR_EL1 Decode: [0x%08x] -> Instruction Abort taken without a change in Exception Level\n", $ESR_EL1
        printf "FAR_EL1 (Faulting Address): 0x%016x\n", $FAR_EL1
    end
    if ($ec == 0x24)
        printf "ESR_EL1 Decode: [0x%08x] -> Data Abort from a lower Exception Level (User-space)\n", $ESR_EL1
        printf "FAR_EL1 (Faulting Address): 0x%016x\n", $FAR_EL1
    end
    if ($ec == 0x25)
        printf "ESR_EL1 Decode: [0x%08x] -> Data Abort taken without a change in Exception Level (Kernel)\n", $ESR_EL1
        printf "FAR_EL1 (Faulting Address): 0x%016x\n", $FAR_EL1
    end
    if ($ec == 0x3C)
        printf "ESR_EL1 Decode: [0x%08x] -> BRK instruction execution (Software Breakpoint)\n", $ESR_EL1
    end
end

# --- 5. Architecture Specific Stop Hook ---
define archsize64_arm64-stop-hook
    echo \n=======================================================\n
    echo [ARM64] CPU Paused. Context:\n
    display /i $pc
    
    # Automatically decode the crash reason using the new Native GDB macro
    decode_esr
    
    # Dump system and general registers
    info_sys_reg
    infr
    
    echo =======================================================\n
end