
#include "BinaryTree.h"

void	insertNumber(t_BT *tree)
{
	int number;
	char buffer[256];

	printf("Enter a number to insert: ");
	fgets(buffer, sizeof(buffer), stdin);
	number = atoi(buffer);
	if (insertNode(&tree, number, FALSE) == TRUE)
		printf(GREEN "Number %d inserted successfully.\n" RESET, number);
	else
		printf(RED "Error inserting number %d.\n" RESET, number);
}

int	insertNode(t_BT **tree, int number, int addedFlag)
{
	if ((*tree) == NULL)
	{
		*tree = newClearNode(*tree);
		(*tree)->number = number;
		return TRUE;
	}
	else if ((number < (*tree)->number) && addedFlag == FALSE) // Insert in the left subtree
		addedFlag = insertNode(&(*tree)->next_left, number, addedFlag);
	else if ((number > (*tree)->number) && addedFlag == FALSE) // Insert in the right subtree
		addedFlag = insertNode(&(*tree)->next_right, number, addedFlag);
	return addedFlag;
}

t_BT	*newClearNode(t_BT *node)
{
	if (node == NULL)
	{
		node = calloc(1, sizeof(t_BT));
		node->number = 0;
		node->next_left = NULL;
		node->next_right = NULL;
	}
	return node;
}

void	removeNumber(t_BT *tree)
{
	int number;
	char buffer[256];

	printf("Enter a number to remove: ");
	fgets(buffer, sizeof(buffer), stdin);
	number = atoi(buffer);
	
	if (tree->number == number)
	{
		removeNodeDoubleParent(tree);
		return;
	}
	else if (removeNode(tree, tree, UNKNOWN, number) == TRUE)
		printf(GREEN "Number %d removed successfully.\n" RESET, number);
	else
		printf(RED "Error removing number %d.\n" RESET, number);
}