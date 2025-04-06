/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 20:03:48 by alexafer          #+#    #+#             */
/*   Updated: 2025/02/02 18:14:22 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_NM_H
# define FT_NM_H

# include <stdio.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include "../libft/libft.h"
# include <elf.h>

/*
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;
*/
// ELF header Ehdr
// ELF Program Phdr
// ELF Section Shdr

typedef struct nm_value
{
	int					flag_a;
	int					flag_g;
	int					flag_u;
	int					flag_r;
	int					flag_p;
	int					flag_m;
	char				*file_path;
	int					index;
	char				c;
	int					padding;
	char				*name_symtab;
	unsigned long long	addr;
	Elf64_Sym			elf64_sym;
	Elf32_Sym			elf32_sym;
}	t_nm_value;

typedef struct elf_file
{
	char		*file_path;
	char		*file_map;
	off_t		file_len;
	off_t		offset;
	off_t		offset_string;
	int			error;
	int			bss;
	int			data;

	uint8_t	type_ehdr;
	uint8_t	type_end;

	int			padding;
	int			flag_a;
	int			flag_g;
	int			flag_u;
	int			flag_r;
	int			flag_p;
	int			flag_m;

	Elf64_Ehdr	*elf64_ehdr;
	Elf32_Ehdr	*elf32_ehdr;

	Elf64_Sym	*elf64_sym;
	Elf32_Sym	*elf32_sym;

	int			number_symtab;
	int			number_dynsym;
	char		**name_symtab;


	Elf64_Phdr **elf64_phdr;
	Elf32_Phdr **elf32_phdr;

	Elf64_Shdr	elf64_shdr_text;
	Elf64_Shdr	**elf64_shdr;
	Elf32_Shdr	**elf32_shdr;

	t_nm_value	*tmp;
	t_list		*lst_value;

}	t_elf_file;

off_t	file_length(char *path);
void	mapping_file(t_elf_file *file);
void	file_used(t_elf_file *file, char *str);
int		elf_parsing(t_elf_file *file);
int		nm_sort_key(char *str);
void	elf64_shdr_parse(t_elf_file *file);
void	elf32_shdr_parse(t_elf_file *file);
void	elf_set(t_elf_file *file, int len, void **value, int end);
void	print_hexa(unsigned long long value, int padding);
void	print_nm(void *value);
//void	ft_swap(t_list *stack, int index, int index2);
void	ft_swap(t_list *f, t_list *s);
t_list	*ft_return_index(t_list *stack, int index);
void	sort_output(t_list *stack);
int		ft_atoi_base(char *str, char *base);
char	*ft_itoa_base(int nbr, char *base);
int		ft_parse(t_elf_file *file, int argc, char **argv, int nb);
void	print_nm32(void *value);
void	ft_free_lst(t_elf_file *file);
void	ft_free_all(t_elf_file *file);
void	index_lst(t_list *lst);
int		ft_parse_nb(int argc, char **argv);
void	ft_elf32_set(t_elf_file *file);
void	ft_elf64_set(t_elf_file *file);
char	elf32_get_symbol(t_elf_file *file, Elf32_Sym sym);
char	elf64_get_symbol(t_elf_file *file, Elf64_Sym sym);
int		check_shdr_bounds(t_elf_file *file);
int		check_shdr_bounds_32(t_elf_file *file);
int		ft_strcmp(const char *s1, const char *s2);
void	elf64_phdr_parse(t_elf_file *file);
void	create_new_file_from_map(t_elf_file *file);
//void	elf_set_tamp(t_elf_file *file, int len, void *value, int end);
uint64_t read_u64(uint8_t *data, int is_big_endian);
uint16_t read_u16(uint8_t *data, int is_big_endian);
uint32_t read_u32(uint8_t *data, int is_big_endian);
void insert_bytes(t_elf_file *file, const char *new_bytes, size_t insert_offset, size_t insert_size, size_t allocated_size);
#endif
