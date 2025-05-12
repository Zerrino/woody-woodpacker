#include <stdio.h>

int	main(int argc, char **argv, char **envp)
{
	int	i = 0;

	printf("ARGC : %d\n", argc);
	while (i < argc)
	{
		printf("argv[%d] : %s\n", i, argv[i]);
		i++;
	}

	// Afficher les premières variables d’environnement
	printf("\nENVIRONNEMENT :\n");
	i = 0;
	while (envp[i])
	{
		//printf("envp[%d] : %s\n", i, envp[i]);
		i++;
	}
	printf("envp[%d]\n", i);

	return 0;
}
