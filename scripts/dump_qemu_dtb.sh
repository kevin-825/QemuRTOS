#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# QEMU binary   BSP board name    machine name
QEMUS=(
    "riscv qemu-system-riscv32  qemu_virt_rv32gc virt"
    "riscv qemu-system-riscv64  qemu_virt_rv64gc virt"
    "arm   qemu-system-arm      qemu_virt_arm    virt"
    "arm   qemu-system-aarch64 qemu_virt_aarch64 virt"
)


dump_one() {
    local arch="$1"
    local QEMU_BIN="$2"
    local BOARD="$3"
    local MACHINE="$4"

    local dts_path="$REPO_ROOT/boards/${arch}/${BOARD}/dts"

    mkdir -p "${dts_path}"

    local DTB="${dts_path}/${BOARD}.dtb"
    local DTS="${dts_path}/${BOARD}.dts"

    echo "[*] Dumping DTB for ${QEMU_BIN} (board=${BOARD}, machine=${MACHINE})"

    "$QEMU_BIN" \
        -machine "${MACHINE},dumpdtb=${DTB}" \
        -nographic

    echo "    Converting DTB → DTS"
    dtc -I dtb -O dts "${DTB}" -o "${DTS}"

    echo "    Output:"
    echo "      ${DTB}"
    echo "      ${DTS}"
}

for entry in "${QEMUS[@]}"; do
    dump_one $entry
done

echo "[*] All DTBs dumped successfully."
