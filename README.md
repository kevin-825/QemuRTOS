# QemuRTOS

QemuRTOS is a lightweight, experimental Real-Time Operating System (RTOS) built specifically to run and be tested within the QEMU emulator. It incorporates modern RTOS design concepts, including a macro-driven, tiered device initialization model and dynamic hardware discovery using Flattened Device Trees (FDT).

## Key Features

* **Device Tree (DTB) Auto-Probing**: Seamless hardware configuration via `libfdt`. Devices are matched against `compatible` strings (e.g., `ns16550a` for UART) dynamically at boot.
* **Tiered Device Initialization**: Devices are initialized in defined tiers (`PRE_KERNEL`, `POST_KERNEL`) using macro declarations (`DEVICE_DEFINE`) similar to the Zephyr RTOS.
* **Modular Driver Architecture**: Clear API abstraction for drivers. The OS currently includes core serial abstractions and an NS16550 UART driver.
* **Developer & Debug Friendly**: Includes shell scripts for auto-generating customized `.gdbinit` files tailored for QEMU target debugging.

## Project Structure

* **`kernel/`**: Core OS logic including the central `device.c` management and booting sequence.
* **`include/`**: Header files defining APIs, device states, and configurations.
* **`drivers/`**: Hardware-specific drivers (e.g., `drivers/serial/ns16550.c`).
* **`lib/`**: External libraries. Contains `libfdt` used for parsing standard Device Tree Blobs.
* **`scripts/`**: Tooling and helper scripts, including the dynamic GDB configuration generator (`auto_gdbinit.sh`).

## Prerequisites

To build and run QemuRTOS, you will need the following tools installed on your host machine:

* A cross-compiler toolchain (e.g., `riscv64-unknown-elf-gcc` or `arm-none-eabi-gcc` depending on your target architecture).
* **QEMU** emulator (`qemu-system-riscv64`, `qemu-system-arm`, etc.)
* **GDB** for your specific architecture (e.g., `gdb-multiarch`).
* **GNU Make** or your configured build system.



## Getting Started

### 1. Building the OS
Compile the OS using your build system. By default, this will generate an ELF executable file (e.g., `qemurtos.elf`) in your build output directory.

```bash
make
```

### 2. Running in QEMU
QemuRTOS relies on QEMU passing a Device Tree Blob (DTB) to the kernel. You can start QEMU with a specific machine type and your compiled kernel. 

*(Example using a generic RISC-V virt machine)*
```bash
qemu-system-riscv64 -machine virt -m 128M -nographic -kernel build/qemurtos.elf
```
> **Note:** Depending on your machine, QEMU might auto-generate the DTB, or you might need to pass one explicitly using the `-dtb path/to/board.dtb` flag.

## Debugging

QemuRTOS is built with seamless GDB debugging in mind. 

### 1. Generate the GDB Init File
Use the provided script to automatically generate a GDB initialization script tailored to your build output and architecture:

```bash
./scripts/auto_gdbinit.sh <PROJECT_ROOT> <BUILD_OUTPUT_DIR> <CROSS_COMPILE> <ELF_PATH> <ARCH> <ARCH_SIZE> <BOARD_DIR>
```
*This script generates an `auto_gdbinit` file inside the `scripts/` folder.*

### 2. Start QEMU in Debug Mode
Run QEMU with the `-s -S` flags to open a GDB server port (usually `:1234`) and halt the CPU at startup:

```bash
qemu-system-riscv64 -machine virt -nographic -kernel build/qemurtos.elf -s -S
```

### 3. Attach GDB
In a new terminal window, start your cross-compiled GDB and pass the generated script:

```bash
gdb-multiarch -x scripts/auto_gdbinit
```

You can now step through the kernel initialization, inspect device trees, and trace driver initialization!

## Extending QemuRTOS

To add a new device driver:
1. Implement your hardware init function and operational API.
2. Register your driver using the `DEVICE_DEFINE` macro at the bottom of your source file.
3. Make sure the `compatible` string in your macro matches the node in the QEMU target's Device Tree.