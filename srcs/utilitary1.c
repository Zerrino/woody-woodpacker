/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilitary1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:49:14 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 07:14:29 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

off_t	file_length(char *path)
{
	struct stat	buffer;
	int			fd_out;
	int			fs_out;

	fd_out = open(path, O_RDONLY);
	if (fd_out < 0)
	{
		ft_putstr_fd("nm: '", 1);
		ft_putstr_fd(path, 1);
		ft_putendl_fd("': No such file", 1);
		return (-1);
	}
	fs_out = fstat(fd_out, &buffer);
	if (fs_out < 0)
	{
		ft_putendl_fd("fstat failed.", 2);
		return (-1);
	}
	close(fd_out);
	if (S_ISDIR(buffer.st_mode))
	{
		ft_putendl_fd("directory not accepeted.", 2);
		return (-1);
	}
	return (buffer.st_size);
}

void insert_bytes(t_elf_file *file, const char *new_bytes, size_t insert_offset, size_t insert_size, size_t allocated_size)
{
	size_t old_size = file->file_len;
	size_t new_size = old_size + insert_size;

	if (new_size > allocated_size)
	{
		ft_putendl_fd("Mapping insuffisant pour l'insertion.", 2);
		return;
	}
	ft_memmove(file->file_map + insert_offset + insert_size,
			file->file_map + insert_offset,
			old_size - insert_offset);

	ft_memcpy(file->file_map + insert_offset, new_bytes, insert_size);

	file->file_len = new_size;
}

void	mapping_file(t_elf_file *file)
{
	int			fd_out;

	fd_out = open(file->file_path, O_RDONLY);
	if (fd_out < 0)
	{
		ft_putendl_fd("error opening file.", 2);
		file->error = 1;
		return ;
	}
	file->file_map = mmap(NULL, file->file_len + 0x38, PROT_READ | PROT_WRITE,
			MAP_PRIVATE, fd_out, 0);
	close(fd_out);
	if (file->file_map == MAP_FAILED)
	{
		ft_putendl_fd("map failed.", 2);
		exit(0);
	}
}

void create_new_file_from_map(t_elf_file *file)
{
	int fd_new;
	ssize_t bytes_written;
	off_t total_written = 0;

	fd_new = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755);
	if (fd_new < 0)
	{
		ft_putendl_fd("Error: could not create new file", 2);
		return;
	}
	while (total_written < file->file_len)
	{
		bytes_written = write(fd_new,
							(char *)file->file_map + total_written,
							file->file_len - total_written);
		if (bytes_written < 0)
		{
			ft_putendl_fd("Error: write failed", 2);
			close(fd_new);
			return;
		}
		total_written += bytes_written;
	}
	close(fd_new);
}

void	print_nm(void *value)
{
	t_nm_value	*val;

	val = value;
	if (ELF64_ST_BIND(val->elf64_sym.st_info) == STB_LOCAL && val->flag_g == 1)
		return ;
	if (val->elf64_sym.st_shndx != 0 && val->flag_u == 1)
		return ;
	if (!val->flag_a
		&& (ELF64_ST_TYPE(val->elf64_sym.st_info) == STT_FILE
			|| ELF64_ST_TYPE(val->elf64_sym.st_info) == STT_SECTION))
		return ;
	if (val->elf64_sym.st_shndx == 0)
		write(1, "                 ", val->padding + 1);
	else
		print_hexa(val->addr, val->padding);
	write(1, &val->c, 1);
	write(1, " ", 1);
	ft_putendl_fd(val->name_symtab, 1);
}

void	print_hexa(unsigned long long value, int padding)
{
	char		str[17];
	int			c;
	char		*hex;
	int			i;

	hex = (char *)"0123456789abcdef";
	i = 0;
	while (i < padding)
	{
		c = 15;
		c &= value;
		value >>= 4;
		str[padding - i - 1] = hex[c];
		i++;
	}
	str[padding] = '\0';
	write(1, &str, padding);
	write(1, " ", 1);
}

void	file_used(t_elf_file *file, char *str)
{
	file->error = 0;
	file->file_len = file_length(str);
	if (file->file_len < 0x34)
	{
		if (file->file_len >= 0)
			ft_putendl_fd("too small to be an elf file.", 2);
		file->error = 1;
		return ;
	}
	file->file_path = str;
	mapping_file(file);
}
