#include "BinaryTree.h"

void	searchPreOrder(t_BT *tree)
{
	if (tree == NULL)
		return;

	printf("%d, ", tree->number);
	if (tree->next_left != NULL)
		searchPreOrder(tree->next_left);

	if (tree->next_right != NULL)
		searchPreOrder(tree->next_right);
}

void	searchInOrder(t_BT *tree)
{
	if (tree == NULL)
		return;
	
	if (tree->next_left != NULL && tree->next_right != NULL)
	{
		searchInOrder(tree->next_left);
		printf("%d, ", tree->number);
		searchInOrder(tree->next_right);
		return;
	}
	if (tree->next_left != NULL)
		searchInOrder(tree->next_left);
	if (tree->next_right != NULL)
		searchInOrder(tree->next_right);
	printf("%d, ", tree->number);
}

void	searchPostOrder(t_BT *tree)
{
	if (tree == NULL)
		return;
	if (tree->next_left == NULL)
		printf("%d, ", tree->number);
	if (tree->next_left != NULL)
		searchPostOrder(tree->next_left);
	if (tree->next_left != NULL)
		printf("%d, ", tree->number);
	if (tree->next_right != NULL)
		searchPostOrder(tree->next_right);
}

void	printTree(t_BT *tree)
{
	if (tree == NULL)
		return;

	printf("Node number: %d (%p)\n", tree->number, (void *)tree);
	if (tree->next_left != NULL)
	{
		printf("Left child of %d (%p): ", tree->number, (void *)tree->next_left);
		printTree(tree->next_left);
	}
	else
		printf("Left child of %d (%p): NULL\n", tree->number, (void *)tree->next_left);

	if (tree->next_right != NULL)
	{
		printf("Right child of %d (%p): ", tree->number, (void *)tree->next_right);
		printTree(tree->next_right);
	}
	else
		printf("Right child of %d (%p): NULL\n", tree->number, (void *)tree->next_right);
}