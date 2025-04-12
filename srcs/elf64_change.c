/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_change.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/04/12 15:30:32 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"
#include <assert.h>

void print_segment_type(uint32_t p_type)
{
    switch (p_type) {
        case 0x00000000: printf("0x%08x\tPT_NULL\tProgram header table entry unused.\n", p_type); break;
        case 0x00000001: printf("0x%08x\tPT_LOAD\tLoadable segment.\n", p_type); break;
        case 0x00000002: printf("0x%08x\tPT_DYNAMIC\tDynamic linking information.\n", p_type); break;
        case 0x00000003: printf("0x%08x\tPT_INTERP\tInterpreter information.\n", p_type); break;
        case 0x00000004: printf("0x%08x\tPT_NOTE\tAuxiliary information.\n", p_type); break;
        case 0x00000005: printf("0x%08x\tPT_SHLIB\tReserved.\n", p_type); break;
        case 0x00000006: printf("0x%08x\tPT_PHDR\tSegment containing program header table itself.\n", p_type); break;
        case 0x00000007: printf("0x%08x\tPT_TLS\tThread-Local Storage template.\n", p_type); break;
        default:
            if (p_type >= 0x60000000 && p_type <= 0x6FFFFFFF)
                printf("0x%08x\tPT_OS_SPECIFIC\tOperating system specific.\n", p_type);
            else if (p_type >= 0x70000000 && p_type <= 0x7FFFFFFF)
                printf("0x%08x\tPT_PROC_SPECIFIC\tProcessor specific.\n", p_type);
            else
                printf("0x%08x\tUNKNOWN\tUnrecognized segment type.\n", p_type);
            break;
    }
}

void create_load_segment_header(Elf64_Phdr *ph, uint64_t file_off, uint64_t size, uint64_t vbase)
{
    // Définition du type du segment : chargement
    ph->p_type   = PT_LOAD;

    // Définition des flags : ici, lecture et écriture (modifiable selon vos besoins)
    ph->p_flags  = PF_R | PF_W;

    // Le segment commencera à l'offset fourni, c'est-à-dire à la fin du fichier
    ph->p_offset = file_off;

    // Calcul de l'adresse virtuelle : la base virtuelle plus l’offset
    ph->p_vaddr  = vbase + file_off;

    // Dans un contexte simple, l'adresse physique est la même que l'adresse virtuelle
    ph->p_paddr  = ph->p_vaddr;

    // La taille dans le fichier et en mémoire est définie à 'size' (ici 10)
    ph->p_filesz = size;
    ph->p_memsz  = size;

    // Alignement du segment : par exemple, au niveau d'une taille de page classique (4096 octets)
    ph->p_align  = 0x1000;
}

void print_segment_flags(unsigned int p_flags)
{
	printf("  Flags: 0x%x (", p_flags);

	int first = 1;
	if (p_flags & PF_R) {
		printf("READ");
		first = 0;
	}
	if (p_flags & PF_W) {
		if (!first) printf(" | ");
		printf("WRITE");
		first = 0;
	}
	if (p_flags & PF_X) {
		if (!first) printf(" | ");
		printf("EXECUTE");
	}

	printf(")\n");
}

void	elf64_hdr_change(t_elf_file *file)
{
	(void)file;
}

void	elf64_shdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_shnum)
	{

		//ft_memset(file->elf64_shdr[i], '\0', sizeof(Elf64_Shdr));
		i++;
	}
}

void xor_crypt(unsigned char *data, unsigned char *key, size_t size_data, size_t size_key)
{
    for (size_t i = 0; i < size_data; i++)
    {
        data[i] ^= key[i % size_key];
    }
}


void generate_key(unsigned char *key, size_t size)
{
    int fd = open("/dev/urandom", O_RDONLY);
    size_t i;
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    if (read(fd, key, size) != (ssize_t)size)
    {
        perror("read");
        close(fd);
        exit(1);
    }
    close(fd);
    i = 0;
    while (i < size)
    {
        key[i] = key[i] % 62;
        if (key[i] < 10)
            key[i] += '0';
        else if (key[i] < 36)
            key[i] += 'A' - 10;
        else
            key[i] += 'a' - 36;
        i++;
    }
}

void elf64_phdr_change(t_elf_file *file)
{
    int i;
    Elf64_Phdr *target_phdr = NULL;
    Elf64_Off max_end = 0;
    for (i = 0; i < file->elf64_ehdr->e_phnum; i++)
    {
        file->elf64_phdr[i]->p_flags |= 0x7;
        if (file->elf64_phdr[i]->p_type == PT_LOAD)
        {
            Elf64_Off end = file->elf64_phdr[i]->p_offset + file->elf64_phdr[i]->p_filesz;
            if (end > max_end)
            {
                max_end = end;
                target_phdr = file->elf64_phdr[i];
            }
        }
    }

    if (!target_phdr)
    {
        fprintf(stderr, "Aucun segment LOAD trouvé\n");
        return;
    }
    target_phdr->p_flags |= PF_X;
    off_t stub_offset = max_end;





    unsigned char key[16];
    generate_key(key, 16);
    write(1, "key : ", 6);
    write(1, key, 16);
    write(1, "\n", 1);

    //_ft_encrypt((unsigned char *)(file->file_map + file->text_offset), key, file->text_offset, sizeof(key));
    //_ft_encrypt((unsigned char *)(file->file_map + file->text_offset), key, file->text_offset, sizeof(key));

// offset : 4288 -> 0x10c0 , size : 501069 -> 0x7a54d


// fichier : 778256 -> 0xbe010


    unsigned char stub[272] = {
        // jmp short +0x0e
        0xeb, 0x0e,
        // ....WOODY....\n
        0x2e, 0x2e, 0x2e, 0x2e,
        0x57, 0x4f, 0x4f, 0x44, 0x59,
        0x2e, 0x2e, 0x2e, 0x2e,
        0x0a,
        // mov eax, 1
        0xB8, 0x01, 0x00, 0x00, 0x00,
        // mov edi, 1
        0xBF, 0x01, 0x00, 0x00, 0x00,
        // lea rsi, [rip - 0]
        0x48, 0x8D, 0x35, 0xE1, 0xFF, 0xFF, 0xFF,
        // mov edx, 14
        0xBA, 0x0e, 0x00, 0x00, 0x00,
        // syscall
        0x0f, 0x05,
        // mov rcx, [rsp]           ; rcx = argc
        0x48, 0x8b, 0x0c, 0x24,
        // cmp rcx, 2               ; if (argc < 2)
        0x48, 0x83, 0xf9, 0x02,
        // jge exit                  ; jump to exit
        0x0f, 0x8d, 0x0d, 0x00, 0x00, 0x00,

        // exit:
        // mov rax, 0x3c
        0x48, 0xC7, 0xC0, 0x3C, 0x00, 0x00, 0x00,
        // xor rdi, rdi
        0x48, 0x31, 0xFF,
        // syscall
        0x0F, 0x05,



        // mov rdi, [rsp]           ; rdi = argc
        0x48, 0x8b, 0x3c, 0x24,
        // lea rsi, [rsp+8]         ; rsi = &argv[0]
        0x48, 0x8d, 0x74, 0x24, 0x08,
        // mov rbx, [rsi]           ; rbx = argv[0] (filename)
        0x48, 0x8b, 0x1e,
        // mov rax, 2               ; syscall number: open
        0x48, 0xc7, 0xc0, 0x02, 0x00, 0x00, 0x00,
        // mov rdi, rbx             ; filename
        0x48, 0x89, 0xdf,
        // mov rsi, 0               ; flags = O_RDONLY
        0x48, 0xc7, 0xc6, 0x00, 0x00, 0x00, 0x00,
        // syscall                  ; open(argv[0], O_RDWR)
        0x0f, 0x05,
        // mov r12, rax             ; save fd ICI


// offset : 4288 -> 0x10c0 , size : 501069 -> 0x7a54d


// fichier : 778256 -> 0xbe010

        0x49, 0x89, 0xc4,
        // xor rdi, rdi
        0x48, 0x31, 0xFF,
        // mov rsi, 0x157 -> donc ici file len 15 520 0x3ca0 fichier : 778256 -> 0xbe010
        0x48, 0xC7, 0xC6, 0x10, 0xe0, 0x0b, 0x00,
        // mov rdx, 0x1
        0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00,
        // mov r10, 0x2
        0x49, 0xC7, 0xC2, 0x02, 0x00, 0x00, 0x00,
        // mov r8, r12
        0x4D, 0x89, 0xE0,
        // mov r9, 0x1060 -> ici on modife pour 0 (pas d'offset) 0x1060 (ou commence le code)
        0x49, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00,
        // mov rax, 0x9
        0x48, 0xC7, 0xC0, 0x09, 0x00, 0x00, 0x00,
        // syscall
        0x0F, 0x05,


// offset : 4288 -> 0x10c0 , size : 501069 -> 0x7a54d


// fichier : 778256 -> 0xbe010

        // mov r13, rax
        0x49, 0x89, 0xC5,
        // xor rdi, rdi
        0x48, 0x31, 0xFF,
        // mov rsi, 0x157 size : 501069 -> 0x7a54d
        0x48, 0xC7, 0xC6, 0x4d, 0xa5, 0x07, 0x00,
        // mov rdx, 0x7
        0x48, 0xC7, 0xC2, 0x07, 0x00, 0x00, 0x00,
        // mov r10, 0x22
        0x49, 0xC7, 0xC2, 0x22, 0x00, 0x00, 0x00,
        // xor r8, r8
        0x4D, 0x31, 0xC0,
        // dec r8
        0x49, 0xFF, 0xC8,
        // xor r9, r9
        0x4D, 0x31, 0xC9,
        // mov rax, 0x9
        0x48, 0xC7, 0xC0, 0x09, 0x00, 0x00, 0x00,
        // syscall
        0x0F, 0x05,



        // mov r14, rax
        0x49, 0x89, 0xC6,

        // mov rcx, 0x157 la len ->
        0x48, 0xC7, 0xC1, 0x4d, 0xa5, 0x07, 0x00,
        // lea rsi = r13 + 0x1060
        0x49, 0x8d, 0xb5, 0xc0, 0x10, 0x00, 0x00,
        // mov rdi, r14
        0x4C, 0x89, 0xF7,

        // memcpy_loop:
        // cmp rcx, 0
        0x48, 0x83, 0xF9, 0x00,
        // je memcpy_finished
        0x74, 0x0F,
        // mov al, [rsi]
        0x8A, 0x06,
        // mov [rdi], al
        0x88, 0x07,
        // inc rsi
        0x48, 0xFF, 0xC6,
        // inc rdi
        0x48, 0xFF, 0xC7,
        // dec rcx
        0x48, 0xFF, 0xC9,
        // jmp memcpy_loop
        0xEB, 0xEB,

        // memcpy_finished:
        // mov rdi, r14
        0x4C, 0x89, 0xF7,
        // mov rsi, [rsp+0x18]
        0x48, 0x8B, 0x74, 0x24, 0x18,
        // mov rdx, 0x157
        0x48, 0xC7, 0xC2, 0x4d, 0xa5, 0x07, 0x00,
        // mov rcx, 0x10
        0x48, 0xC7, 0xC1, 0x10, 0x00, 0x00, 0x00,
        // xor r8, r8
        0x4D, 0x31, 0xC0,
        // xor r9, r9
        0x4D, 0x31, 0xC9,

        /*
        // decrypt_loop:
        // cmp r8, rdx
        0x49, 0x39, 0xD0,
        // je decrypt_end
        0x74, 0x1F,
        // mov r10b, [rsi+r9]
        0x46, 0x8A, 0x14, 0x0E,
        // mov r11b, [rdi+r8]
        0x46, 0x8A, 0x1C, 0x07,
        // xor r11b, r10b
        0x45, 0x30, 0xD3,
        // mov [rdi+r8], r11b
        0x46, 0x88, 0x1C, 0x07,
        // inc r8
        0x49, 0xFF, 0xC0,
        // inc r9
        0x49, 0xFF, 0xC1,
        // cmp r9, rcx
        0x49, 0x39, 0xC9,
        // jne decrypt_loop
        0x75, 0xE1,
        // xor r9, r9
        0x4D, 0x31, 0xC9,
        // jmp decrypt_loop
        0xEB, 0xDC,
        */

        // mov rdi, r12
        0x4C, 0x89, 0xE7,
        // mov rax, 0x3
        0x48, 0xC7, 0xC0, 0x03, 0x00, 0x00, 0x00,
        // syscall
        0x0F, 0x05,

        /*
        // mov rcx, [rsp+0x8]
        0x48, 0x8B, 0x4C, 0x24, 0x08,
        // lea rbx, [rsp+0x8]
        0x48, 0x8D, 0x5C, 0x24, 0x08,

        // shift_argv:
        // mov rax, [rbx+0x18]
        0x48, 0x8B, 0x43, 0x18,
        // mov [rbx+0x8], rax
        0x48, 0x89, 0x43, 0x08,
        // add rbx, 0x8
        0x48, 0x83, 0xC3, 0x08,
        // dec rcx
        0x48, 0xFF, 0xC9,
        // cmp rcx, 0x1
        0x48, 0x83, 0xF9, 0x01,
        // jg shift_argv
        0x7F, 0xEB,

        // mov [rbx+0x8], 0
        0x48, 0xC7, 0x43, 0x08, 0x00, 0x00, 0x00, 0x00,
        */

        // jmp r14
        0x41, 0xFF, 0xE6,
    };



    Elf64_Addr stub_vaddr = (target_phdr->p_vaddr + (stub_offset - target_phdr->p_offset));
    //int32_t relative_offset = file->elf64_ehdr->e_entry - (stub_vaddr + sizeof(stub));
    //ft_memcpy(&stub[sizeof(stub) - 4], &relative_offset, sizeof(uint32_t));
    ft_memcpy(file->file_map + stub_offset, stub, sizeof(stub));
    if ((long unsigned int)file->file_len < (stub_offset + sizeof(stub)))
        file->file_len = stub_offset + sizeof(stub);
    target_phdr->p_filesz += sizeof(stub);
    target_phdr->p_memsz  += sizeof(stub);
    file->elf64_ehdr->e_entry = stub_vaddr;
}


void	elf64_woody(t_elf_file *file)
{
	elf64_hdr_change(file);
	elf64_shdr_change(file);
	elf64_phdr_change(file);
	create_new_file_from_map(file);
}
