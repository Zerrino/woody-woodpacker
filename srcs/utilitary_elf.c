/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilitary_elf.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 07:15:48 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

void	ft_elf64_set(t_elf_file *file)
{
	elf_set(file, 2, &file->elf64_ehdr.e_type, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_machine, 0);
	elf_set(file, 4, &file->elf64_ehdr.e_version, 0);
	elf_set(file, 8, &file->elf64_ehdr.e_entry, 0);
	elf_set(file, 8, &file->elf64_ehdr.e_phoff, 0);
	elf_set(file, 8, &file->elf64_ehdr.e_shoff, 0);
	elf_set(file, 4, &file->elf64_ehdr.e_flags, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_ehsize, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_phentsize, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_phnum, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_shentsize, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_shnum, 0);
	elf_set(file, 2, &file->elf64_ehdr.e_shstrndx, 0);
}

void	ft_elf32_set(t_elf_file *file)
{
	elf_set(file, 2, &file->elf32_ehdr.e_type, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_machine, 0);
	elf_set(file, 4, &file->elf32_ehdr.e_version, 0);
	elf_set(file, 4, &file->elf32_ehdr.e_entry, 0);
	elf_set(file, 4, &file->elf32_ehdr.e_phoff, 0);
	elf_set(file, 4, &file->elf32_ehdr.e_shoff, 0);
	elf_set(file, 4, &file->elf32_ehdr.e_flags, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_ehsize, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_phentsize, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_phnum, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_shentsize, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_shnum, 0);
	elf_set(file, 2, &file->elf32_ehdr.e_shstrndx, 0);
}

int	check_shdr_bounds(t_elf_file *file)
{
	if ((off_t)((file->elf64_ehdr.e_shnum * 64) + file->elf64_ehdr.e_shoff)
		> file->file_len)
		return (0);
	return (1);
}

int	check_shdr_bounds_32(t_elf_file *file)
{
	if ((off_t)((file->elf32_ehdr.e_shnum * 40) + file->elf32_ehdr.e_shoff)
		> file->file_len)
		return (0);
	return (1);
}

void	ft_swap(t_list *f, t_list *s)
{
	void	*tmp;

	tmp = s->content;
	s->content = f->content;
	f->content = tmp;
}
