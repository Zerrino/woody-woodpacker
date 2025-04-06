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
// On se place au début du mapping pour l'en-tête ELF
	file->elf64_ehdr = (Elf64_Ehdr *)file->file_map;
}

void	ft_elf32_set(t_elf_file *file)
{
	file->elf32_ehdr = (Elf32_Ehdr *)file->file_map;
}

int	check_shdr_bounds(t_elf_file *file)
{
	if ((off_t)((file->elf64_ehdr->e_shnum * 64) + file->elf64_ehdr->e_shoff)
		> file->file_len)
		return (0);
	return (1);
}

int	check_shdr_bounds_32(t_elf_file *file)
{
	if ((off_t)((file->elf32_ehdr->e_shnum * 40) + file->elf32_ehdr->e_shoff)
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
