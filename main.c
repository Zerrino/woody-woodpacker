int _start(void)
{
    // write(1, "Hello\n", 6);
    asm volatile(
        "mov $1, %%rax\n\t"
        "mov $1, %%rdi\n\t"
        "lea message(%%rip), %%rsi\n\t"
        "mov $6, %%rdx\n\t"
        "syscall\n\t"
        "mov $60, %%rax\n\t"
        "xor %%rdi, %%rdi\n\t"
        "syscall\n\t"
        "message: .asciz \"Hello\\n\"\n\t"
        :
        :
        : "rax", "rdi", "rsi", "rdx"
    );
    return 0;
}

