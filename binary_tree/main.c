
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