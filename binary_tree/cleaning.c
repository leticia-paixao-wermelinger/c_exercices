#include "BinaryTree.h"

void	clear_all(t_BT *tree)
{

	if (tree == NULL)
		return;

	if (tree->next_left != NULL)
		clear_all(tree->next_left);
	if (tree->next_right != NULL)
		clear_all(tree->next_right);

	printf("Freeing node with number: %d\n", tree->number);
	if (tree)
	{
		free(tree);
		tree = NULL;
	}
	return;	
}