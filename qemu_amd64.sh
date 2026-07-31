qemu-system-x86_64 \
  	-cpu max \
	-m 1G \
	-serial stdio \
	-drive if=pflash,format=raw,readonly=on,file=EFI_X64.fd \
	-drive if=pflash,format=raw,file=NVRAM_X64.fd \
	-drive format=raw,file=fat:rw:esp \
	-device qemu-xhci,id=xhci -device usb-kbd,bus=xhci.0 \
	-d int,cpu_reset,guest_errors \
	-D qemu.log \
	-device virtio-gpu-pci \
	-s
	
