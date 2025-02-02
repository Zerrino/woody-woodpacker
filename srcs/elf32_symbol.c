/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf32_symbol.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:53:13 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 07:00:36 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

static char	get_ifunc_symbol(unsigned char bind)
{
	if (bind == STB_LOCAL)
		return ('i');
	return ('I');
}

static char	get_weak_symbol(unsigned short shndx, unsigned char type)
{
	if (shndx == SHN_UNDEF)
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

static char	get_special_section_symbol(Elf32_Sym sym)
{
	if (sym.st_shndx == SHN_UNDEF)
		return ('U');
	else if (sym.st_shndx == SHN_ABS)
		return ('A');
	else if (sym.st_shndx == SHN_COMMON)
		return ('C');
	return ('?');
}

static char	get_section_symbol(t_elf_file *file, Elf32_Sym sym)
{
	Elf32_Shdr	*sec;
	const char	*sec_name;

	sec = &file->elf32_shdr[sym.st_shndx];
	sec_name = (const char *)
		(file->file_map + file->elf32_shdr
		[file->elf32_ehdr.e_shstrndx].sh_offset + sec->sh_name);
	if (!ft_strncmp(sec_name, ".text", ft_strlen(sec_name)))
		return ('T');
	else if (!ft_strncmp(sec_name, ".data", ft_strlen(sec_name)))
		return ('D');
	else if (!ft_strncmp(sec_name, ".bss", ft_strlen(sec_name)))
		return ('B');
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
	return ('N');
}

char	elf32_get_symbol(t_elf_file *file, Elf32_Sym sym)
{
	char			letter;
	unsigned char	bind;
	unsigned char	type;

	letter = '?';
	bind = ELF32_ST_BIND(sym.st_info);
	type = ELF32_ST_TYPE(sym.st_info);
	if (type == STT_GNU_IFUNC)
		return (get_ifunc_symbol(bind));
	if (bind == STB_WEAK)
		return (get_weak_symbol(sym.st_shndx, type));
	if (sym.st_shndx == SHN_UNDEF || sym.st_shndx == SHN_ABS
		|| sym.st_shndx == SHN_COMMON)
		letter = get_special_section_symbol(sym);
	else if (sym.st_shndx < file->elf32_ehdr.e_shnum)
		letter = get_section_symbol(file, sym);
	if (bind == STB_LOCAL && letter != '?')
		letter |= 32;
	return (letter);
}
