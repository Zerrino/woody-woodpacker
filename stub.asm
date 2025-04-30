; stub.asm
BITS 64

section .bss
    buffer: resb 4096

section .data
    name:   db "aaaaaaa", 0
    file:   db "./second", 0
    exec:   db "/proc/self/fd/", 0

section .text
global _start

_start:
    ; Sauvegarder les pointeurs d'arguments et d'environnement
    ; À l'entrée, la pile contient :
    ;   [rsp]         = argc
    ;   [rsp+8]       = argv[0]
    ;   ...
    ;   [rsp+8*(argc+1)] = NULL (fin de argv)
    ;   [rsp+8*(argc+2)] = envp[0]
    mov rbx, rsp           ; sauvegarde rsp d'origine (non utilisé ultérieurement ici)
    lea r12, [rsp+8]       ; r12 pointe sur argv (tel que passé à l'exécution)
    mov rax, [rsp]         ; rax = argc
    mov rcx, rax
    add rcx, 2             ; rcx = argc + 2
    shl rcx, 3             ; rcx = 8 * (argc + 2)
    lea r13, [rsp+rcx]     ; r13 pointe sur envp

    ; Créer un memfd via sys_memfd_create
    mov rax, 319           ; sys_memfd_create
    mov rdi, name          ; nom aléatoire du fichier
    xor rsi, rsi           ; flags = 0
    syscall
    test rax, rax         ; tester si erreur (< 0)
    js error
    mov r15, rax          ; stocker le fd du memfd dans r15

    ; Ouvrir le fichier ELF "./second"
    mov rax, 2            ; syscall open
    mov rdi, file
    xor rsi, rsi
    syscall
    test rax, rax        ; tester si erreur (< 0)
    js error
    mov r14, rax          ; stocker le fd du fichier "./second" dans r14

read_loop:
    mov rax, 0          ; SYS_read
    mov rdi, r14        ; fd du fichier "./second"
    mov rsi, buffer     ; buffer pour stocker les données lues
    mov rdx, 4096       ; taille max à lire
    syscall
    test rax, rax      ; fin de fichier (lecture retourne 0)
    jz done_read
    js error           ; en cas d'erreur lors de la lecture

    ; Écrire dans le memfd le nombre d'octets lus
    mov rdi, r15       ; fd du memfd
    mov rdx, rax       ; nombre d'octets lus
    mov rax, 1         ; SYS_write
    syscall
    jmp read_loop

done_read:
    ; Construire le chemin "/proc/self/fd/<fd>"
    ; rsi pointera à la fin de la chaîne "/proc/self/fd/"
    mov rsi, exec
    add rsi, 14        ; on se place à la fin de la chaîne de base

    ; Convertir le fd (dans r15) en chaîne ASCII et l'ajouter à exec.
    mov rax, r15      ; fd à convertir
    xor rcx, rcx      ; compteur de chiffres
convert_fd:
    xor rdx, rdx
    mov rbx, 10
    div rbx           ; divise rax par 10: quotient dans rax, reste dans rdx
    add dl, '0'       ; convertir le reste en caractère ASCII
    push rdx          ; sauvegarder le caractère (les chiffres seront ainsi dans l'ordre inverse)
    inc rcx           ; incrémenter le compteur de chiffres
    test rax, rax
    jnz convert_fd

write_fd:
    pop rax
    mov byte [rsi], al
    inc rsi
    loop write_fd

    mov byte [rsi], 0

    ; Appeler execve en utilisant les mêmes argc, argv et envp que le stub initial
    mov rax, 59         ; syscall execve
    mov rdi, exec       ; chemin vers l'exécutable ("/proc/self/fd/<fd>")
    mov rsi, r12        ; argv original tel que passé à ce programme
    mov rdx, r13        ; envp original
    syscall

exit:
    mov rax, 60
    xor rdi, rdi
    syscall

error:
    mov rax, 60
    mov rdi, 1
    syscall
