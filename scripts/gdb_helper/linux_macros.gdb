# ==============================================================================
# LINUX KERNEL SOURCE-LEVEL HELPERS (linux_macros.gdb)
# ==============================================================================
# Description: Architecture-agnostic macros for debugging Linux kernel state.
# Requires: Kernel compiled with CONFIG_GDB_SCRIPTS=y



# Warning: Locking the scheduler in SMP Linux can trigger kernel watchdogs.
# Use carefully when stepping through lock-sensitive code.
define schl_on
    set scheduler-locking on
    echo [\n] Scheduler Locking: ON\n
end

define schl_off
    set scheduler-locking off
    echo [\n] Scheduler Locking: OFF\n
end

# --- 2. Process & State Inspection ---
define cur
    echo [\n] Current Task Struct (Process running on active CPU):\n
    lx-current
end

define ps
    echo [\n] Active Process List:\n
    lx-ps
end

define lsmod
    echo [\n] Loaded Kernel Modules:\n
    lx-lsmod
end

define dmesg
    echo [\n] Dumping Kernel Log Buffer (dmesg):\n
    lx-dmesg
end

# --- 3. Symbol & Address Resolution ---
define resolve
    # Instantly tells you what function or variable lives at a raw hex address
    # Usage: resolve 0xffffffc000123456
    info symbol $arg0
end

# --- 4. The Universal List Walker ---
# Linux connects almost everything (tasks, modules, devices) via `struct list_head`.
# This macro traverses any standard kernel list and prints the addresses.
#
# Usage: walk_list <list_head_ptr> <struct_type> <member_name>
# Example (Iterate all modules): walk_list &modules "module" "list"
define walk_list
    if $argc != 3
        echo Usage: walk_list <list_head_ptr> <struct_type> <member_name>\n
    else
        set $head = (struct list_head *)$arg0
        set $curr = $head->next
        
        # Calculate the offset of the list_head inside the parent struct
        set $offset = (unsigned long)&(((struct $arg1 *)0)->$arg2)
        
        set $count = 0
        echo [\n] Walking list at $head...\n
        
        while $curr != $head
            # Subtract the offset to get the base pointer of the actual struct
            set $entry = (struct $arg1 *)((unsigned long)$curr - $offset)
            printf "[%d] struct %s @ 0x%lx\n", $count, "$arg1", $entry
            
            # Move to the next item
            set $curr = $curr->next
            set $count = $count + 1
        end
        printf "Total items: %d\n\n", $count
    end
end

# --- 5. Boot/Crash Helpers ---
define catch_crash
    # Automatically dumps the log buffer if the kernel panics
    hbreak panic
    commands
        echo [\n] KERNEL PANIC CAUGHT! Dumping logs...\n
        dmesg
    end
end