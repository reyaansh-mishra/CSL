qemu-system-aarch64 \
  	-machine virt,virtualization=on,secure=off \
  	-cpu max \
	-m 4G \
	-serial stdio \
	-drive if=pflash,format=raw,readonly=on,file=EFI.fd \
	-drive if=pflash,format=raw,file=NVRAM.fd \
	-drive format=raw,file=fat:rw:esp \
	-device virtio-gpu-pci \
	-device qemu-xhci,id=xhci -device usb-kbd,bus=xhci.0 \
	-d int,cpu_reset,guest_errors \
	-D qemu.log \

