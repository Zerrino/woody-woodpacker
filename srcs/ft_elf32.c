/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_elf32.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 15:51:05 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 07:08:42 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

static int	parse_section_headers_32(t_elf_file *file, int *sym_link)
{
	int	i;
	int	sym_index;

	sym_index = -1;
	i = 0;
	while (i < file->elf32_ehdr.e_shnum)
	{
		elf_set(file, 4, &file->elf32_shdr[i].sh_name, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_type, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_flags, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_addr, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_offset, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_size, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_link, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_info, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_addralign, 0);
		elf_set(file, 4, &file->elf32_shdr[i].sh_entsize, 0);
		if (file->elf32_shdr[i].sh_type == SHT_SYMTAB)
		{
			sym_index = i;
			*sym_link = file->elf32_shdr[i].sh_link;
		}
		i++;
	}
	return (sym_index);
}

static int	allocate_symbol_arrays_32(t_elf_file *file, int sym_index)
{
	file->number_symtab = file->elf32_shdr[sym_index].sh_size
		/ file->elf32_shdr[sym_index].sh_entsize;
	file->tmp = (t_nm_value *)malloc(sizeof(t_nm_value)
			* (file->number_symtab - 1));
	if (!file->tmp)
		return (0);
	file->elf32_sym = (Elf32_Sym *)malloc(sizeof(Elf32_Sym)
			* file->number_symtab);
	if (!file->elf32_sym)
		return (0);
	file->offset = file->elf32_shdr[sym_index].sh_offset;
	return (1);
}

static void	process_symbol_entry_32(t_elf_file *file, int i, int sym_link)
{
	file->tmp[i - 1].c = elf32_get_symbol(file, file->elf32_sym[i]);
	if (ELF32_ST_TYPE(file->elf32_sym[i].st_info) == 3)
		file->tmp[i - 1].name_symtab = file->elf32_shdr
		[file->elf32_sym[i].st_shndx].sh_name + file->file_map
			+ file->elf32_shdr[file->elf32_ehdr.e_shstrndx].sh_offset;
	else
		file->tmp[i - 1].name_symtab = file->file_map
			+ file->elf32_shdr[sym_link].sh_offset + file->elf32_sym[i].st_name;
	file->tmp[i - 1].padding = file->padding;
	file->tmp[i - 1].addr = file->elf32_sym[i].st_value;
	file->tmp[i - 1].elf32_sym = file->elf32_sym[i];
	file->tmp[i - 1].flag_a = file->flag_a;
	file->tmp[i - 1].flag_g = file->flag_g;
	file->tmp[i - 1].flag_u = file->flag_u;
	file->tmp[i - 1].flag_r = file->flag_r;
	file->tmp[i - 1].flag_p = file->flag_p;
	file->tmp[i - 1].flag_m = file->flag_m;
	file->tmp[i - 1].file_path = file->file_path;
	ft_lstadd_back(&file->lst_value, ft_lstnew(&file->tmp[i - 1]));
}

static void	parse_symbol_table_entries_32(t_elf_file *file, int sym_link)
{
	int	i;

	i = 0;
	while (i < file->number_symtab)
	{
		elf_set(file, sizeof(file->elf32_sym[i].st_name),
			&file->elf32_sym[i].st_name, 0);
		elf_set(file, sizeof(file->elf32_sym[i].st_value),
			&file->elf32_sym[i].st_value, 0);
		elf_set(file, sizeof(file->elf32_sym[i].st_size),
			&file->elf32_sym[i].st_size, 0);
		elf_set(file, sizeof(file->elf32_sym[i].st_info),
			&file->elf32_sym[i].st_info, 0);
		elf_set(file, sizeof(file->elf32_sym[i].st_other),
			&file->elf32_sym[i].st_other, 0);
		elf_set(file, sizeof(file->elf32_sym[i].st_shndx),
			&file->elf32_sym[i].st_shndx, 0);
		if (i > 0)
			process_symbol_entry_32(file, i, sym_link);
		i++;
	}
}

void	elf32_shdr_parse(t_elf_file *file)
{
	int	sym_index;
	int	sym_link;

	if (!check_shdr_bounds_32(file))
		return ;
	sym_index = parse_section_headers_32(file, &sym_link);
	if (sym_index == -1)
	{
		ft_putstr_fd("nm: ", 1);
		ft_putstr_fd(file->file_path, 1);
		ft_putendl_fd(": no symbols", 1);
		return ;
	}
	if (!allocate_symbol_arrays_32(file, sym_index))
		return ;
	parse_symbol_table_entries_32(file, sym_link);
}
