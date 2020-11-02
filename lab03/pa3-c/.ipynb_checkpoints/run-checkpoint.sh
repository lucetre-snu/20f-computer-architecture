riscv32-unknown-elf-gcc -Ofast -c test.c
riscv32-unknown-elf-objdump -d test.o
gcc test.c pa1-sol.c pa1-test.c main.c -o main
./main