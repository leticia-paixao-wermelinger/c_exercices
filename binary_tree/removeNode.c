
#include "BinaryTree.h"

int	removeNode(t_BT *prev, t_BT *node, int flagSide, int number)
{
	int	ret = FALSE;
//	printf("Entrou em removeNode com número: %d e node (%p) de número %d\n", number, (void *)node, node->number);
//	printf("Prev (%p) de número %d\n", (void *)prev, prev->number);

	if (ret == FALSE)
	{
		if (node->next_left && node->next_right && number != node->number) // Search in both subtrees
		{
			ret = removeNode(node, node->next_left, LEFT, number);
			if (ret == FALSE)
				ret = removeNode(node, node->next_right, RIGHT, number);
		}
		else if (node->next_left && number != node->number) // Search in the left subtree
		{
//			printf("Searching in the left subtree for number: %d\n", number);
			ret = removeNode(node, node->next_left, LEFT, number);
		}
		else if (node->next_right && number != node->number) // Search in the right subtree
		{
//			printf("Searching in the right subtree for number: %d\n", number);
			ret = removeNode(node, node->next_right, RIGHT, number);
		}
		else if (number == node->number) // Node found
		{
//			printf("Found node with number: %d\n", node->number);
			if (node->next_left == NULL && node->next_right == NULL) // Leaf node
			{
				printf("Removing leaf node: %d\n", node->number);
				if (flagSide == LEFT)
					prev->next_left = NULL;
				else if (flagSide == RIGHT)
					prev->next_right = NULL;
				free(node);
				ret = TRUE;
			}
			else if (node->next_left == NULL) // Only right child
			{
				printf("Removing node with only right child, number: %d\n", node->number);
				t_BT *temp = node->next_right;
				if (flagSide == LEFT)
					prev->next_left = temp;
				else if (flagSide == RIGHT)
					prev->next_right = temp;
				
				free(node);
				ret = TRUE;
			}
			else if (node->next_right == NULL) // Only left child
			{
				printf("Removing node with only left child, number: %d\n", node->number);
				t_BT *temp = node->next_left;
				if (flagSide == LEFT)
					prev->next_left = temp;
				else if (flagSide == RIGHT)
					prev->next_right = temp;
				free(node);
				ret = TRUE;
			}
			else // Node with two children // TA ERRADO! Descobrir como remover um nó com dois filhos
			{
				printf(RED "Cannot remove a node with two children.\n" RESET);
				ret = FALSE;
			}
		}
		else if (node->number != number)
			ret = FALSE; // Node not found, but continue searching
		else
		{
			printf("Node with number %d is not removable.\n", node->number);
			ret = FALSE; // Node not removable
		}
	}
	return ret; // Node not found or not removable
}