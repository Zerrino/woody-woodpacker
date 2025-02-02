/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilitary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 09:49:14 by Zerrino           #+#    #+#             */
/*   Updated: 2025/02/02 07:15:46 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

void	sort_output(t_list *lst)
{
	t_nm_value	*v1;
	t_nm_value	*v2;
	t_list		*b;
	int			cmp;

	index_lst(lst);
	while (lst)
	{
		b = lst->next;
		while (b)
		{
			v1 = (t_nm_value *)lst->content;
			v2 = (t_nm_value *)b->content;
			cmp = ft_strcmp(v1->name_symtab, v2->name_symtab);
			if (v1->flag_r && cmp < 0)
				ft_swap(lst, b);
			else if (!v1->flag_r && cmp > 0)
				ft_swap(lst, b);
			else if (cmp == 0)
				if (v1->index > v2->index)
					ft_swap(lst, b);
			b = b->next;
		}
		lst = lst->next;
	}
}

static void	parse_flag_argument(t_elf_file *file, char *arg)
{
	if (ft_strchr(arg, 'a') && ft_strchr(arg, 'a')[0] == 'a')
		file->flag_a = 1;
	if (ft_strchr(arg, 'g') && ft_strchr(arg, 'g')[0] == 'g')
		file->flag_g = 1;
	if (ft_strchr(arg, 'u') && ft_strchr(arg, 'u')[0] == 'u')
		file->flag_u = 1;
	if (ft_strchr(arg, 'r') && ft_strchr(arg, 'r')[0] == 'r')
		file->flag_r = 1;
	if (ft_strchr(arg, 'p') && ft_strchr(arg, 'p')[0] == 'p')
		file->flag_p = 1;
}

static void	parse_file_argument(t_elf_file *file, char *arg, int *nb, int *f)
{
	(*nb)--;
	if (*nb == 0)
		file_used(file, arg);
	*f = 1;
}

int	ft_parse(t_elf_file *file, int argc, char **argv, int nb)
{
	int	i;
	int	f;

	f = 0;
	if (argc == 1)
		file_used(file, "./a.out");
	else
	{
		i = 1;
		while (i < argc)
		{
			if (argv[i][0] == '-')
				parse_flag_argument(file, argv[i]);
			else
				parse_file_argument(file, argv[i], &nb, &f);
			i++;
		}
		if (f == 0)
			file_used(file, "./a.out");
	}
	return (file->error);
}

int	ft_parse_nb(int argc, char **argv)
{
	int	i;
	int	f;

	i = 1;
	f = 0;
	while (i < argc)
	{
		if (argv[i][0] != '-')
			f++;
		i++;
	}
	return (f);
}
