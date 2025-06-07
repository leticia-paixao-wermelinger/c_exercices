/**
* Universidade Veiga de Almeida
* Curso: Análise e Desenvolvimento de Sistemas
* Disciplina: Estrutura de De Dados (E90100017)
* Professor: Professor Marco Aurelio Novaes Esteves
* Estudante: Letícia Paixão Wermelinger
* Matrícula: 1240210571
* Data: 06/06/2025
*/

#include "BinaryTree.h"

int	removeNode(t_BT *prev, t_BT *node, int flagSide, int number)
{
	if (prev == NULL)
		return FALSE;

	if (number < prev->next_left->number) // Search in the left subtree
		return removeNode(prev, prev->next_left, LEFT, number);
	else if (number > prev->next_right->number) // Search in the right subtree
		return removeNode(prev, prev->next_right, RIGHT, number);
	else // Node found
	{
		if (node->next_left == NULL && node->next_right == NULL) // Leaf node
		{
			free(node);
			if (flagSide == LEFT)
				prev->next_left = NULL;
			else if (flagSide == RIGHT)
				prev->next_right = NULL;
			return TRUE;
		}
		else if (prev->next_left == NULL) // Only right child
		{
			t_BT *temp = node->next_right;
			free(node);
			if (flagSide == LEFT)
				prev->next_left = temp;
			else if (flagSide == RIGHT)
				prev->next_right = temp;
			return TRUE;
		}
		else if (prev->next_right == NULL) // Only left child
		{
			t_BT *temp = node->next_right;
			free(node);
			if (flagSide == LEFT)
				prev->next_left = temp;
			else if (flagSide == RIGHT)
				prev->next_right = temp;
			return TRUE;
		}
		else // Node with two children // TA ERRADO! Descobrir como remover um nó com dois filhos
		{
			printf(RED "Cannot remove a node with two children.\n" RESET);
			return FALSE;
		}
	}
}