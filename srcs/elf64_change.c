/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_change.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/04/11 00:50:24 by Zerrino          ###   ########.fr       */
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

    xor_crypt((unsigned char *)(file->file_map + file->text_offset), key, file->text_offset, sizeof(key));

    unsigned char stub[54] = {
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






        // xor rax, rax
        0x48, 0x31, 0xC0,
        // xor rdi, rdi
        0x48, 0x31, 0xFF,
        // xor rdx, rdx
        0x48, 0x31, 0xD2,
        // jmp [rip + 0]
        0xE9, 0x00, 0x00, 0x00, 0x00
    };


    Elf64_Addr stub_vaddr = (target_phdr->p_vaddr + (stub_offset - target_phdr->p_offset));
    int32_t relative_offset = file->elf64_ehdr->e_entry - (stub_vaddr + sizeof(stub));
    ft_memcpy(&stub[sizeof(stub) - 4], &relative_offset, sizeof(uint32_t));
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
