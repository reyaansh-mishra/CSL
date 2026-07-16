qemu-system-aarch64 \
  	-machine virt \
  	-cpu cortex-a57 \
	-drive if=pflash,format=raw,readonly=on,file=EFI.fd \
	-drive if=pflash,format=raw,file=NVRAM.fd \
	-drive format=raw,file=fat:rw:esp
