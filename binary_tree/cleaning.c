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