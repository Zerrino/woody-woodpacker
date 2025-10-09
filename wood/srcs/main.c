/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zerrino <zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 20:21:36 by Zerrino           #+#    #+#             */
/*   Updated: 2025/10/09 19:36:54 by zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_nm.h"

int	main(int argc, char **argv)
{
	int			i;
	int			nb;
	t_elf_file	*file;

	if (argc != 2)
		return (1);
	nb = ft_parse_nb(argc, argv) + (1 >> ft_parse_nb(argc, argv));
	file = (t_elf_file *)malloc(sizeof(t_elf_file) * nb);
	if (!file)
		return (0);
	i = 0;
	while (i < nb)
	{
		ft_bzero(&file[i], sizeof(file));
		if (nb > 1)
			file[i].flag_m = 1;
		if (!ft_parse(&file[i], argc, argv, i + 1))
		{
			if (file[i].error == 0)
				elf_parsing(&file[i]);
			if (file[i].error == 0)
			{
				munmap(file[i].file_map, file[i].file_len);
			}
		}
		i++;
	}
	free(file);
	return (0);
}
