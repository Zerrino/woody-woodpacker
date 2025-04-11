
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("pas de argc == 2!\n");
	}
	printf("argv[0] : %s, argc : %d\n", argv[0], argc);
	printf("Salut!\n");
	exit(0);
}
