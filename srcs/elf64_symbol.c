/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64_symbol.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 17:56:08 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

static char	get_ifunc_symbol_64(unsigned char bind)
{
	if (bind == STB_LOCAL)
		return ('I');
	return ('i');
}

static char	get_weak_symbol_64(Elf64_Sym sym, unsigned char type)
{
	if (sym.st_shndx == SHN_UNDEF)
	{
		if (type == STT_OBJECT)
			return ('v');
		return ('w');
	}
	else
	{
		if (type == STT_OBJECT)
			return ('V');
		return ('W');
	}
}

static char	get_special_section_symbol_64(Elf64_Sym sym)
{
	if (sym.st_shndx == SHN_UNDEF)
		return ('U');
	else if (sym.st_shndx == SHN_ABS)
		return ('A');
	else if (sym.st_shndx == SHN_COMMON)
		return ('C');
	return ('?');
}

static char	get_section_symbol_64(t_elf_file *file, Elf64_Sym sym)
{
	Elf64_Shdr	*sec;
	const char	*sec_name;

	sec = file->elf64_shdr[sym.st_shndx];
	sec_name = (const char *)(file->file_map + file->elf64_shdr
		[file->elf64_ehdr->e_shstrndx]->sh_offset + sec->sh_name);
	if (!ft_strncmp(sec_name, ".text", ft_strlen(sec_name)))
		return ('T');
	else if (!ft_strncmp(sec_name, ".bss", ft_strlen(sec_name)))
		return ('B');
	else if (!ft_strncmp(sec_name, ".data", ft_strlen(sec_name)))
		return ('D');
	else if (!ft_strncmp(sec_name, ".rodata", ft_strlen(sec_name)))
		return ('R');
	if (sec->sh_type == SHT_NOBITS && (sec->sh_flags & SHF_ALLOC)
		&& (sec->sh_flags & SHF_WRITE))
		return ('B');
	else if (sec->sh_flags & SHF_EXECINSTR)
		return ('T');
	else if ((sec->sh_flags & SHF_ALLOC) && (sec->sh_flags & SHF_WRITE))
		return ('D');
	else if (sec->sh_flags & SHF_ALLOC)
		return ('R');
	else
		return ('N');
}

char	elf64_get_symbol(t_elf_file *file, Elf64_Sym sym)
{
	char			letter;
	unsigned char	bind;
	unsigned char	type;

	letter = '?';
	bind = ELF64_ST_BIND(sym.st_info);
	type = ELF64_ST_TYPE(sym.st_info);
	if (type == STT_GNU_IFUNC)
		return (get_ifunc_symbol_64(bind));
	if (bind == STB_WEAK)
		return (get_weak_symbol_64(sym, type));
	if (sym.st_shndx == SHN_UNDEF || sym.st_shndx == SHN_ABS
		|| sym.st_shndx == SHN_COMMON)
		letter = get_special_section_symbol_64(sym);
	else if (sym.st_shndx < file->elf64_ehdr->e_shnum)
		letter = get_section_symbol_64(file, sym);
	if (bind == STB_LOCAL && letter != '?')
		letter |= 32;
	return (letter);
}
