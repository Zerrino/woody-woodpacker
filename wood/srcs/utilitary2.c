/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilitary2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:49:14 by Zerrino           #+#    #+#             */
/*   Updated: 2025/04/06 18:25:12 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

void	print_nm32(void *value)
{
	t_nm_value	*val;

	val = value;
	if (ELF32_ST_BIND(val->elf32_sym.st_info) == STB_LOCAL && val->flag_g == 1)
		return ;
	if (val->elf32_sym.st_shndx != 0 && val->flag_u == 1)
		return ;
	if (!val->flag_a
		&& (ELF32_ST_TYPE(val->elf32_sym.st_info) == STT_FILE
			|| ELF32_ST_TYPE(val->elf32_sym.st_info) == STT_SECTION))
		return ;
	if (val->elf32_sym.st_shndx == 0)
		write(1, "                 ", val->padding + 1);
	else
		print_hexa(val->addr, val->padding);
	write(1, &val->c, 1);
	write(1, " ", 1);
	ft_putendl_fd(val->name_symtab, 1);
}

void	ft_free_all(t_elf_file *file)
{
	if (file->lst_value)
		ft_free_lst(file);
	if (file->tmp)
		free(file->tmp);
	if (file->elf64_shdr)
		free(file->elf64_shdr);
	if (file->elf64_sym)
		free(file->elf64_sym);
	if (file->elf32_shdr)
		free(file->elf32_shdr);
	if (file->elf32_sym)
		free(file->elf32_sym);
	if (file->elf32_phdr)
		free(file->elf32_phdr);
	if (file->elf64_phdr)
		free(file->elf64_phdr);
}

void	ft_free_lst(t_elf_file *file)
{
	t_list	*tmp;

	if (ft_lstsize(file->lst_value) > 0)
	{
		tmp = file->lst_value->next;
		while (tmp)
		{
			free(file->lst_value);
			file->lst_value = tmp;
			tmp = tmp->next;
		}
		free(file->lst_value);
	}
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

void	index_lst(t_list *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		((t_nm_value *)lst->content)->index = i;
		lst = lst->next;
		i++;
	}
}
