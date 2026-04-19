#!/usr/bin/env fish

qemu-system-aarch64 \
  -M virt \
  -cpu cortex-a53 \
  -m 1024 \
  -bios /usr/share/edk2/aarch64/QEMU_EFI.fd \
  -kernel kernel \
  -initrd initramfs.img \
  -append "console=ttyAMA0 init=/init" \
  -netdev user,id=net0 \
  -device e1000,netdev=net0 \
  -drive file=disk.img,format=raw,if=virtio \
  -nographic \
  -display none \
  -serial mon:stdio
