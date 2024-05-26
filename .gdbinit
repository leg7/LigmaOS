define hook-stop
	printf "[%4x:%4x] ", $cs, $eip
	x/i $cs*16+$eip
end

define os-setup
	target remote localhost:26000
	file src/isodir/boot/os.bin
end

define os-start
	os-setup
	break kernel_main
	continue
end

define os-restart
	os-setup
	continue
end

set disassembly-flavor intel
set architecture i8086
os-start

alias g = os-restart
