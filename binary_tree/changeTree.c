
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
	if ((*tree) == NULL) // If the current node is empty or NULL
	{
		*tree = newClearNode(*tree);
		(*tree)->number = number; // Set the number for the new node
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
		node->number = 0; // Initialize new node
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
	
	// VOLTAR AQUI DEPOIS!! Descobrir o que fazer se o número for o nó raiz:

	if (tree->number == number) // DESCOBRIR O Q FAZER NESSE CASO
	{
		printf(RED "Cannot remove the root node directly. Please remove it from the tree.\n" RESET);
		return;
	}
	else if (removeNode(tree, tree, UNKNOWN, number) == TRUE)
		printf(GREEN "Number %d removed successfully.\n" RESET, number);
	else
		printf(RED "Error removing number %d.\n" RESET, number);
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