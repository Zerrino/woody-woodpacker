/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_change.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/04/07 22:45:00 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

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

void adjust_segment_fields(Elf64_Phdr *ph, Elf64_Off x)
{
    // On laisse p_vaddr inchangé (et de même p_paddr si nécessaire)
    // On augmente p_offset de x pour obtenir le nouvel offset dans le fichier.
    ph->p_offset += x;

    // Calcul de la différence : elle doit être un multiple de new_p_align.
    // Note : On calcule le module de la différence en valeur absolue.
    int64_t diff = (int64_t)(ph->p_offset) - (int64_t)(ph->p_vaddr);
    if(diff < 0)
        diff = -diff;

    // new_p_align doit diviser exactement diff.
    // La formule "diff & (-diff)" donne la plus grande puissance de 2 qui divise diff.
    ph->p_align = (Elf64_Off) diff & (-(Elf64_Off) diff);
}

void	elf64_hdr_change(t_elf_file *file)
{
	if (file->elf64_ehdr->e_shoff > file->offset_insert)
		file->elf64_ehdr->e_shoff += 0x38;
	if (file->elf64_ehdr->e_entry > file->offset_insert)
		file->elf64_ehdr->e_entry += 0x38;
}

void	elf64_shdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_shnum)
	{

		file->elf64_shdr[i]->sh_offset += 0x38;
		file->elf64_shdr[i]->sh_addr   += 0x38;
		i++;
	}
}

void	elf64_phdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_phnum)
	{
		Elf64_Phdr *ph = file->elf64_phdr[i];


		printf("\n[PHDR #%d]\n", i);
		print_segment_type(ph->p_type);
		printf("  Original p_offset  : 0x%lx\n", ph->p_offset);
		printf("  Original p_vaddr   : 0x%lx\n", ph->p_vaddr);
		printf("  Original p_align   : 0x%lx\n", ph->p_align);
		printf("  Insertion point    : 0x%lx\n", file->offset_insert);

		if (ph->p_offset >= file->offset_insert)
		{
			printf("  ➜ p_offset modifié : 0x%lx → ", ph->p_offset);
			ph->p_offset += 0x38;
			ph->p_vaddr  += 0x38;
			ph->p_paddr  += 0x38;
			printf("0x%lx (décalé de +0x38)\n", ph->p_offset);
		}

		if (ph->p_filesz > ph->p_memsz) {
			printf("❌ ERROR: p_filesz > p_memsz in segment #%d\n", i);
		}

		if ((ph->p_offset % ph->p_align) != (ph->p_vaddr % ph->p_align)) {
			ph->p_align = compute_p_align(ph->p_vaddr, ph->p_offset);
		}
		if ((ph->p_offset % ph->p_align) != (ph->p_vaddr % ph->p_align)) {
			printf("❌ ERROR: p_offset %% p_align != p_vaddr %% p_align in segment #%d\n", i);
		}

		if (ph->p_align == 0 || (ph->p_align & (ph->p_align - 1)) != 0) {
			printf("❌ ERROR: p_align is not a power of 2 in segment #%d\n", i);
		}
		i++;
	}
}

void	elf64_woody(t_elf_file *file)
{
	elf64_hdr_change(file);
	elf64_shdr_change(file);
	elf64_phdr_change(file);
	create_new_file_from_map(file);
}
