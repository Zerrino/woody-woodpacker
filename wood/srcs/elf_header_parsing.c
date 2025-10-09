/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_header_parsing.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zerrino <zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/10/09 19:36:22 by zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

/*
void	elf_set_tamp(t_elf_file *file, int len, void *value, int end)
{
	char	*str;
	int		i;

	if (file->type_end == 1 || end == 1)
	{
		i = 0;
		str = (char *)value;
		while (i < len && file->file_len > (i + file->offset))
		{
			str[i] = *(file->file_map + i + file->offset);
			i++;
		}
	}
	else
	{
		i = len - 1;
		str = (char *)value;
		while (i >= 0 && file->file_len > (i + file->offset))
		{
			str[len - i - 1] = *(file->file_map + i + file->offset);
			i--;
		}
	}
	file->offset += len;
}
*/


void	elf_set(t_elf_file *file, int len, void **value, int end)
{
	(void)end;
	*value = file->file_map + file->offset;
	file->offset += len;
}

int	check_header(char buffer[10])
{
	int	error;

	error = 0;
	error += (buffer[0] != 1);
	error += (buffer[1] < 0 || buffer[1] > 0x12);
	error += buffer[3] + buffer[4] + buffer[5] + buffer[6]
		+ buffer[7] + buffer[8] + buffer[9];
	return (error);
}

void	ft_elf64(t_elf_file *file)
{
	file->padding = 16;
	ft_elf64_set(file);
	file->offset = file->elf64_ehdr->e_shoff;
	file->elf64_shdr = (Elf64_Shdr **)malloc
		(sizeof(Elf64_Shdr *) * file->elf64_ehdr->e_shnum);
	if (!file->elf64_shdr)
		return ;
	file->elf64_phdr = (Elf64_Phdr **)malloc
		(sizeof(Elf64_Phdr *) * file->elf64_ehdr->e_phnum);
	if (!file->elf64_phdr)
		return ;
	//printf("nombre de program header : %d\n", file->elf64_ehdr->e_phnum);
	elf64_phdr_parse(file);
	file->offset = file->elf64_ehdr->e_shoff;
	elf64_shdr_parse(file);
	elf64_woody(file);
	//ft_free_all(file);
}

void	ft_elf32(t_elf_file *file)
{
	file->padding = 8;
	ft_elf32_set(file);
	file->offset = file->elf32_ehdr->e_shoff;
	file->elf32_shdr = (Elf32_Shdr **)malloc
		(sizeof(Elf32_Shdr *) * file->elf32_ehdr->e_shnum);
	if (!file->elf32_shdr)
		return ;
	if (file->flag_m)
	{
		write(1, "\n", 1);
		ft_putstr_fd(file->file_path, 1);
		ft_putendl_fd(":", 1);
	}
	elf32_shdr_parse(file);
	if (file->flag_p == 0)
		sort_output(file->lst_value);
	//ft_lstiter(file->lst_value, &print_nm32);
	ft_free_all(file);
}

int	elf_parsing(t_elf_file *file)
{
	int		result;
	char	buffer[10];

	file->offset = 0;
	file->type_end = 1;
	result = ft_strncmp(file->file_map + file->offset, "\x7f\x45\x4c\x46", 4);
	if (result != 0)
		return (0);
	file->offset += 4;
	elf_set(file, 1, (void **)&file->type_ehdr, 1);
	elf_set(file, 1, (void **)&file->type_end, 1);
	//if (file->type_ehdr != 1 && file->type_ehdr != 2)
	//	return (1);
	//if (file->type_end != 1 && file->type_end != 2)
	//	return (1);
	elf_set(file, 10, (void **)&buffer, 1);
	//if (check_header(buffer) != 0)
	//	return (1);
	//if (file->type_ehdr == 0)
	ft_elf64(file);
	return (0);
}
