
# GDB Interface

gdb is a work-in-progress, but these are some useful commands:


To run dromajo with gdb waiting:

```
../build/dromajo --gdbinit 8080 ./uart_test
```

To connect to the gdb_stub:
```
(gdb) file uart_test
(gdb) set debug remote 1
(gdb) target remote localhost:8080
```

To see the registers in the CPU:
```
(gdb) info registers
```

