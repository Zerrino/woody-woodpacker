/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_elf64.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zerrino <zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 15:51:05 by Zerrino           #+#    #+#             */
/*   Updated: 2025/10/09 19:36:16 by zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"



static int	parse_section_headers(t_elf_file *file, int *sym_link)
{
	int	i;
	int	sym_index;
	const char	*name;

	sym_index = -1;
	i = 0;
	file->offset = file->elf64_ehdr->e_shoff;
	while (i < file->elf64_ehdr->e_shnum)
	{
		elf_set(file, file->elf64_ehdr->e_shentsize , (void **)&file->elf64_shdr[i], 0);
		//file->elf64_shdr[i]->sh_addralign = compute_p_align(file->elf64_shdr[i]->sh_addr, file->elf64_shdr[i]->sh_offset, file->elf64_shdr[i]->sh_addralign);
		if (file->elf64_shdr[i]->sh_type == SHT_SYMTAB)
		{
			sym_index = i;
			*sym_link = file->elf64_shdr[i]->sh_link;
		}
		i++;
	}
	i = 0;
	while (i < file->elf64_ehdr->e_shnum)
	{
		name = (file->elf64_shdr[i]->sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr->e_shstrndx]->sh_offset);
		if (ft_strcmp(name , ".text") == 0 && ft_strlen(".text") == 5)
		{
			//printf("Name[%d] : %s\n",i, file->elf64_shdr[i]->sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr->e_shstrndx]->sh_offset);
			file->elf64_shdr_text = *file->elf64_shdr[i];
		}
		//printf("Name[%d] : %s\n",i, file->elf64_shdr[i]->sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr->e_shstrndx]->sh_offset);
		i++;
	}
	return (sym_index);
}

static void	parse_program_headers(t_elf_file *file)
{
	int	i;

	i = 0;
	file->offset = file->elf64_ehdr->e_phoff;
	//printf("file->offset : %ld\n", file->offset);
	while (i < file->elf64_ehdr->e_phnum)
	{
		elf_set(file, file->elf64_ehdr->e_phentsize, (void **)&file->elf64_phdr[i], 0);
		i++;
	}
	//const char *salut = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	//insert_bytes(file, salut, file->offset, 0x38, file->file_len + 0x38);
	//file->elf64_ehdr->e_phnum += 1;
	//file->elf64_ehdr->e_shoff += 0x38;
	//file->elf64_ehdr->e_entry += 0x38;
	//file->offset += 0x38;

}

void	elf64_phdr_parse(t_elf_file *file)
{
	parse_program_headers(file);
	file->offset_insert = file->elf64_ehdr->e_phoff + file->elf64_ehdr->e_phentsize * file->elf64_ehdr->e_phnum;
	//char salut[56];

	//ft_memset(salut, '0', 56);
	//printf("offset to insert : %ld\n", file->offset_insert);
	//insert_bytes(file, salut, file->offset_insert, 0x38, file->file_len + 0x38);
	//printf("test : %d %d\n", file->elf64_phdr[0]->p_type, 0);
}

void	elf64_shdr_parse(t_elf_file *file)
{
	//int			i;
	int			sym_link;
	//char		*start;
	if (!check_shdr_bounds(file))
		return ;
	parse_section_headers(file, &sym_link);
	//printf("offset : %ld, size : %ld\n", file->elf64_shdr_text.sh_offset, file->elf64_shdr_text.sh_size);
	//start = (void *)(file->file_map + file->elf64_shdr_text.sh_offset);
	file->text_offset = file->elf64_shdr_text.sh_offset;
	file->text_size = file->elf64_shdr_text.sh_size;
	/*
	i = 0;
	ft_putnbr_fd(0, 1);
	write(1, "  : ", 4);
	while (i < (int)file->elf64_shdr_text.sh_size)
	{
		if (i > 0 && i % 16 == 0)
		{
			write(1, "\n", 1);
			ft_putnbr_fd(i / 16, 1);
			if (i / 16 < 10)
				write(1, "  : ", 4);
			else
				write(1, " : ", 3);
		}
		print_hexa(start[i], 2);
		i++;
	}
	write(1, "\n", 1);
	*/
}
