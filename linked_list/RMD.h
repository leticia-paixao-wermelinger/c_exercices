#ifndef	RMD_H
#define	RMD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define GREEN "\033[0;32m"
# define RESET "\033[0m"
# define RED "\033[31m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define PINK "\033[35m"

enum e_operations
{
	ADD = 1,
	REMOVE,
	PRINT,
	CHANGE,
	EXIT
};

typedef struct s_RMD
{
	int		id;
	char	*name;
	void 	*next;
}			t_RMD;

void	getFirstUserInput(t_RMD *data);
void	operations(t_RMD **data);
void	addNode(t_RMD *data);
void	removeNode(t_RMD **data);
void	printList(t_RMD *data);
void	changeNode(t_RMD *data);
int		chooseOperation();
void	clear_all(t_RMD **data);
void	free_node(t_RMD *node);
#endif