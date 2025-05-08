all: main

CFLAGS=-march=rv64g -mabi=lp64 -mcmodel=medany

main: crt.S main.c link.ld
	riscv64-unknown-elf-gcc -o main -march=rv64g -mabi=lp64 -static -mcmodel=medany -nostdlib -nostartfiles main.c crt.S -lgcc -Tlink.ld

clean:
	rm -f main
