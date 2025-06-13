
#include "BinaryTree.h"

void	createTree(t_BT *tree)
{
	int numbers[] = {53, 60, 87, 86, 21, 5, 53, 44, 68, 56};
	//int *numbers = getNumbers();

	int i = 1;

//	printf("Ponteiro recebido em createTree: %p\n", tree);
	if (tree == NULL)
	{
		printf(RED "Error: tree is NULL\n" RESET);
		return;
	}
	tree->number = numbers[0];
	while (i < INITIAL_NODES)
	{
		//printf("\n\n");
		insertNode(&tree, numbers[i],  FALSE);
		//printf("\n\n");
		i++;
	}
	//free(numbers);
}

int	*getNumbers()
{
	int *numbers = malloc(sizeof(int) * INITIAL_NODES);
	int i;

	srand(time(NULL));
	printf("Generating random numbers:\n");
	for (i = 0; i < 10; i++)
	{
		numbers[i] = rand() % 100;
		printf("%d, ", numbers[i]);
	}
	printf("\n");
	return numbers;
}