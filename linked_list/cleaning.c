#include "RMD.h"

void	clear_all(t_RMD **data)
{
	if (*data == NULL)
		return;

	t_RMD *temp = *data;

	while (*data)
	{
		temp = (*data)->next;
		free_node(*data);
		*data = temp;
	}
}

void	free_node(t_RMD *node)
{
	if (node == NULL)
		return;

	free(node->name);
	free(node);
}