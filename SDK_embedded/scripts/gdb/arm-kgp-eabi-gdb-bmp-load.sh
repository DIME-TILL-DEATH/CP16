#! /bin/sh

if [ "$1" = "" ] ; then
	arm-kgp-eabi-gdb \
			-ex "target extended /dev/kgp/bmp" \
			-ex "mon swdp_scan" \
			-ex "att 1" 2>&1
else
	arm-kgp-eabi-gdb \
			-ex "target extended /dev/kgp/bmp" \
			-ex "exec-file $1" \
			-ex "mon swdp_scan" \
			-ex "att 1" \
			-ex "load $1" \
			-ex "compare-sections" \
			-ex "kill inferiors 1" \
			-ex "q"
			2>&1

fi

