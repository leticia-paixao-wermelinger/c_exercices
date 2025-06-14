
#include "BinaryTree.h"

int	removeNode(t_BT *prev, t_BT *node, int flagSide, int number)
{
	int	ret = FALSE;

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
			ret = removeNode(node, node->next_left, LEFT, number);
		}
		else if (node->next_right && number != node->number) // Search in the right subtree
		{
			ret = removeNode(node, node->next_right, RIGHT, number);
		}
		else if (number == node->number) // Node found
		{
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
			else // Node with two children
			{
				removeNodeDoubleParent(node);
				ret = TRUE;
			}
		}
		else if (node->number != number)
			ret = FALSE;
		else
		{
			printf("Node with number %d is not removable.\n", node->number);
			ret = FALSE;
		}
	}
	return ret;
}

void removeNodeDoubleParent(t_BT *node)
{
	t_BT	*temp = node->next_right;
	t_BT	*prev = node;
	
	if (temp->next_left != NULL)
	{
		while (temp->next_left != NULL)
		{
		prev = temp;
			temp = temp->next_left;
		}
		prev->next_left = temp->next_right;
	}
	else
		prev->next_right = temp->next_right;
	node->number = temp->number;
	free(temp);
}