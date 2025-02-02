#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void my_function() {
    printf("Hello, World!\n");
}

int main() {
    void (*func_ptr)() = my_function;

    // Changer les permissions mémoire pour rendre la fonction modifiable
    mprotect((void*)((uintptr_t)func_ptr & ~0xFFF), 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);

    // Modifier une instruction dans my_function (remplace "Hello, World!" par "Hacked!")
    memcpy((void*)func_ptr, "\x48\x31\xc0\xc3", 4); // Injecte "xor rax, rax; ret"

    // Exécuter la fonction modifiée
    func_ptr();

    return 0;
}

