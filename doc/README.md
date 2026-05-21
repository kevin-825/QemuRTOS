QemuRTOS/
├── Makefile                 # Top-level Kbuild Makefile
├── Kconfig                  # Top-level Kconfig
├── .docker/                 # WSL2 docker-ce environment
│
├── arch/                    # CPU Core Architecture ONLY
│   ├── arm/
│   │   ├── core/            # Context switch, fatal fault handlers
│   │   └── mpu/             # ARM MPU region programming
│   └── riscv/
│       ├── core/            # Trap entry/exit, CSR manipulation
│       └── pmp/             # RISC-V Physical Memory Protection
│
├── chip/                     # Silicon-on-Chip (The emulated chip)
│   └── qemu/
│       ├── virt_arm/        # QEMU ARM SoC memory maps & init
│       └── virt_riscv/      # QEMU RISC-V SoC memory maps & init
│
├── boards/                  # The specific hardware targets
│   ├── arm/
│   │   └── qemu_virt_a53/   # Board init code and qemu-virt-arm.dts
│   └── riscv/
│       └── qemu_virt_rv64/  # Board init code and qemu-virt-riscv.dts
│
├── kernel/                  # The Swappable OS Cartridge (FreeRTOS / Baremetal)
│
├── drivers/                 # Hardware-agnostic drivers
│   ├── serial/              # pl011.c, ns16550.c
│   └── interrupt_controller/# irq-gic.c, irq-plic.c
│
├── subsys/                  # Middleware (The kernel addition)
│   ├── logging/             # System-wide printk/logging logic
│   └── memory/              # Your TLSF allocator lives here now!
│
├── include/                 # Public APIs (kernel/kernel.h, kernel/device.h)
└── scripts/                 # Build system plumbing (Kconfig tools)