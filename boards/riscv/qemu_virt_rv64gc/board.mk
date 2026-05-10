BOARD_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

QEMU_RUN_CMD_PREFIX := \
qemu-system-riscv64 -nographic -machine virt -net none \
-cpu rv64,zba=true,zbb=true,v=true,vlen=256,vext_spec=v1.0,rvv_ta_all_1s=true,rvv_ma_all_1s=true \
-chardev stdio,id=con,mux=on \
-serial chardev:con -mon chardev=con,mode=readline \
-bios none -smp 4 \
