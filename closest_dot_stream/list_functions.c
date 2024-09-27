#include "closest_dot.h"

t_dots	*create_first_dot_node(char *str, t_dots *list)
{
	char	*s;

	list = (t_dots *)malloc(sizeof(t_dots));
	s = my_strcdup(str, ',');
	list->x = my_atoi(s);
	free(s);
	s = fromstrcdup(str, ',');
	list->y = my_atoi(s);
	free(s);
	list->next = NULL;
	return (list);
}

t_dots	*create_last_dot_node(char *str, t_dots *prev)
{
	char		*s;
	t_dots		*new;

	new = (t_dots *)malloc(sizeof(t_dots));
	prev->next = new;
	s = my_strcdup(str, ',');
	new->x = my_atoi(s);
	free(s);
	s = fromstrcdup(str, ',');
	new->y = my_atoi(s);
	free(s);
	new->next = NULL;
	return (new);
}

t_dots	*create_first_ret_node(t_dots *dot, t_dots *list)
{
	list = (t_dots *)malloc(sizeof(t_dots));
	list->x = dot->x;
	list->y = dot->y;
	list->hypotenuse = dot->hypotenuse;
	list->next = NULL;
	return (list);
}

t_dots	*create_last_ret_node(t_dots *dot, t_dots *prev)
{
	t_dots	*new;

	new = (t_dots *)malloc(sizeof(t_dots));
	new->x = dot->x;
	new->y = dot->y;
	new->hypotenuse = dot->hypotenuse;
	prev->next = new;
	new->next = NULL;
	return (new);
}

void	clear_list(t_dots *node)
{
	t_dots	*temp;

	while (node)
	{
		temp = node;
		node = node->next;
		free(temp);
	}
}

void	print_list(t_dots *list)
{
	t_dots	*temp;

	temp = list;
	while (temp)
	{
		printf("x: %i & ", temp->x);
		printf("y: %i\n", temp->y);
		temp = temp->next;
	}
}
