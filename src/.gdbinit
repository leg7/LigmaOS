define hook-stop
	printf "[%4x:%4x] ", $cs, $eip
	x/i $cs*16+$eip
end

set disassembly-flavor intel
set architecture i8086
target remote localhost:26000