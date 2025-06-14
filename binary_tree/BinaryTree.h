#ifndef	BINARYTREE_H
#define	BINARYTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

# define GREEN "\033[0;32m"
# define RESET "\033[0m"
# define RED "\033[31m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define PINK "\033[35m"

#define INITIAL_NODES 40

enum e_boolean
{
	FALSE = 0,
	TRUE
};

enum e_side
{
	UNKNOWN,
	LEFT,
	RIGHT
};

enum e_operations
{
	CLOSE = 0,
	INSERT,
	REMOVE,
	SEARCH_PRE,
	SEARCH_ORDER,
	SEARCH_POST
};

typedef struct s_BT
{
	int				number;
	struct s_BT 	*next_left;
	struct s_BT 	*next_right;
}			t_BT;

void	createTree(t_BT *tree);
int		*getNumbers();
int		insertNode(t_BT **tree, int number, int addedFlag);
t_BT	*newClearNode(t_BT *node);
void	printTree(t_BT *tree);
void	clear_all(t_BT *tree);
void	insertNumber(t_BT *tree);
void	removeNumber(t_BT *tree);
int		removeNode(t_BT *tree, t_BT *next, int flagSide, int number);
void	removeNodeDoubleParent(t_BT *node);
void	searchPreOrder(t_BT *tree);
void	searchInOrder(t_BT *tree);
void	searchPostOrder(t_BT *tree);
void	options(t_BT **tree);
int		chooseOption();
#endif