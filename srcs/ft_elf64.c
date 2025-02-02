/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_elf64.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 15:51:05 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 18:50:55 by Zerrino          ###   ########.fr       */
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
	while (i < file->elf64_ehdr.e_shnum)
	{
		elf_set(file, 4, &file->elf64_shdr[i].sh_name, 0);
		elf_set(file, 4, &file->elf64_shdr[i].sh_type, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_flags, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_addr, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_offset, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_size, 0);
		elf_set(file, 4, &file->elf64_shdr[i].sh_link, 0);
		elf_set(file, 4, &file->elf64_shdr[i].sh_info, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_addralign, 0);
		elf_set(file, 8, &file->elf64_shdr[i].sh_entsize, 0);
		if (file->elf64_shdr[i].sh_type == SHT_SYMTAB)
		{
			sym_index = i;
			*sym_link = file->elf64_shdr[i].sh_link;
		}
		i++;
	}
	i = 0;
	while (i < file->elf64_ehdr.e_shnum)
	{
		name = (file->elf64_shdr[i].sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr.e_shstrndx].sh_offset);
		if (ft_strcmp(name , ".text") == 0 && ft_strlen(".text") == 5)
		{
			printf("Name[%d] : %s\n",i, file->elf64_shdr[i].sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr.e_shstrndx].sh_offset);
			file->elf64_shdr_text = file->elf64_shdr[i];
		}
		//printf("Name[%d] : %s\n",i, file->elf64_shdr[i].sh_name + file->file_map + file->elf64_shdr[file->elf64_ehdr.e_shstrndx].sh_offset);
		i++;
	}
	return (sym_index);
}

void	elf64_shdr_parse(t_elf_file *file)
{
	int			i;
	int			sym_link;
	char		*start;
	if (!check_shdr_bounds(file))
		return ;
	parse_section_headers(file, &sym_link);
	printf("offset : %ld, size : %ld\n", file->elf64_shdr_text.sh_offset, file->elf64_shdr_text.sh_size);
	start = (void *)(file->file_map + file->elf64_shdr_text.sh_offset);
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
}
