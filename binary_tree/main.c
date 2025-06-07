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

int	main()
{
	t_BT	*tree = NULL;

	tree = calloc(1, sizeof(t_BT));
//	printf("Head da minha árvore: %p\n", tree);
	createTree(tree);
//	printTree(tree);
	options(&tree);
	clear_all(tree);
	return (0);
}