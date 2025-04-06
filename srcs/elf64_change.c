/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_change.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/04/06 19:31:39 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

void	elf64_hdr_change(t_elf_file *file)
{
	if (file->elf64_ehdr->e_shoff > file->offset_insert)
		file->elf64_ehdr->e_shoff += 0x38;
	//if (file->elf64_ehdr->e_entry > file->offset_insert)
		//file->elf64_ehdr->e_entry += 0x38;
}

void	elf64_shdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_shnum)
	{
		printf("sh_offset: %ld > offset_insert : %ld\n", file->elf64_shdr[i]->sh_offset, file->offset_insert);
		if (file->elf64_shdr[i]->sh_offset >= file->offset_insert)
			file->elf64_shdr[i]->sh_offset += 0x38;
		//file->elf64_shdr[i]->sh_addralign = compute_p_align(file->elf64_shdr[i]->sh_addr, file->elf64_shdr[i]->sh_offset, file->elf64_shdr[i]->sh_addralign);
		i++;
	}
}

void	elf64_phdr_change(t_elf_file *file)
{
	int	i;

	i = 0;
	while (i < file->elf64_ehdr->e_phnum)
	{
		printf("ph_offset: %ld > offset_insert : %ld\n", file->elf64_phdr[i]->p_offset, file->offset_insert);
		if (file->elf64_phdr[i]->p_offset > file->offset_insert)
			file->elf64_phdr[i]->p_offset += 0x38;
		file->elf64_phdr[i]->p_align = compute_p_align(file->elf64_phdr[i]->p_vaddr, file->elf64_phdr[i]->p_offset, file->elf64_phdr[i]->p_align);
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
